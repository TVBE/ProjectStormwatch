// Copyright (c) 2022-present Barrelhouse. All rights reserved.
// Written by Tim Verberne.

#include "PlayerCameraController.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterController.h"
#include "PlayerMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetMathLibrary.h"
#if WITH_EDITOR
#include "Editor/UnrealEd/Public/Editor.h"
#endif

UPlayerCameraController::UPlayerCameraController()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerCameraController::OnRegister()
{
	Super::OnRegister();
		
	if (!GetPlayerCharacter()) { return; }
	HeadSocketTransform = GetPlayerCharacter()->GetMesh()->GetSocketTransform("head", RTS_Actor);
}

void UPlayerCameraController::BeginPlay()
{
	Super::BeginPlay();
	
	if (const APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		const FLinearColor Color(0.0f, 0.0f, 0.0f, 1.0f);
		PlayerController->PlayerCameraManager->SetManualCameraFade(1.0f, Color, false);

		if (APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager)
		{
			CameraManager->ViewPitchMax = MaximumViewPitch;
			CameraManager->ViewPitchMin = MinimumViewPitch;
		}
	}
}

void UPlayerCameraController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (GetPlayerCharacter() && GetPlayerCharacterController())
		if (UCameraComponent* Camera {GetPlayerCharacter()->GetCamera()})
	{
		UpdateCameraRotation(Camera, DeltaTime); /** Even with camera sway and centripetal rotation disabled, we need to call this function every frame to update the actual orientation of the camera. */
		UpdateCameraLocation(Camera);
		if (IsDynamicFOVEnabled)
		{
			UpdateCameraFieldOfView(Camera, DeltaTime);
		}
		if (IsDynamicVignetteEnabled)
		{
			UpdateCameraVignetteIntensity(Camera, DeltaTime);
		}
		if (IsDynamicDOFEnabled)
		{
			UpdateCameraDepthOfField(Camera, DeltaTime);
		}
	}
}

// Called by TickComponent.
void UPlayerCameraController::UpdateCameraLocation(UCameraComponent* Camera)
{
	/** Get an alpha value based on the pitch of the camera. We do not want the camera to explicitly follow the head socket if the body of the player isn't visible (e.g. looking down),
	 as this could be perceived as annoying by the user. */ 
	const double PitchAlpha
	{FMath::GetMappedRangeValueClamped(FVector2d(-30.0, -55.0), FVector2d(0.0, 1.0), Camera->GetComponentRotation().Pitch)};
	
	/** Get the delta position of the current head socket location in relation to the default location. This allows us to introduce some socket-bound headbobbing with scalable intensity. */
	const FVector SocketLocation
	{FVector(0, 0,(GetPlayerCharacter()->GetMesh()->GetSocketTransform("head", RTS_Actor).GetLocation() - HeadSocketTransform.GetLocation()).Z * 0.5)};
	
	FVector Result;
	/** If the player is looking forward or up, we don't need to perform any additional calculations and can set the relative location to the CameraConfiguration's default value. */
	if (PitchAlpha == 0.0)
	{
		Result = CameraOffset + (SocketLocation * !GetPlayerCharacter()->GetIsTurningInPlace());
	}
	else
	{
		/** Get the target location if the player is not looking down. */
		const FVector UprightCameraLocation {CameraOffset + (SocketLocation * !GetPlayerCharacter()->GetIsTurningInPlace())};

		/** Get the character's forward vector. */
		const FVector ForwardVector = GetPlayerCharacter()->GetActorForwardVector();

		/** Project the character's velocity onto its forward vector to get the forward velocity. */
		const float ForwardVelocity = FVector::DotProduct(GetPlayerCharacter()->GetVelocity(), ForwardVector);
		
		/** Calculate the target location if the player is looking down. */
		const FVector DownwardCameraLocation {GetPlayerCharacter()->GetMesh()->GetSocketTransform("head", RTS_Actor).GetLocation() + FVector(CameraOffset.X * 0.625, 0, 0)
		- FVector(0, 0, (ForwardVelocity * 0.02))}; // We lower the camera slightly when the character is moving forward to simulate the body leaning forward.
		
		/** Interpolate between the two target locations depending on PitchAlpha. */
		Result = FMath::Lerp(UprightCameraLocation, DownwardCameraLocation, PitchAlpha); //NOTE: In UE 5.1 using FMath::Lerp() with two FVectors can cause semantic errors, but the code will compile and run just fine.
	}
	/** Rotate the result with the base aim rotation. */
	const FRotator ControlRotation {FRotator(0, PlayerCharacterController->GetPlayerControlRotation().Yaw, 0)};
	Result = ControlRotation.RotateVector(Result);
	
	/** Add the world location of the pawn to the result. */
	Result = Result + GetPlayerCharacter()->GetActorLocation();
	
	Camera->SetWorldLocation(Result);
}

/** Called by TickComponent. */
void UPlayerCameraController::UpdateCameraRotation(const UCameraComponent*, const float DeltaTime)
{
	if (IsCameraSwayEnabled)
	{
		GetCameraSwayRotation(Sway);
	}
	
	if (IsCentripetalRotationEnabled)
	{
		GetCameraCentripetalRotation(CentripetalRotation);
	}
	
	if (!GetPlayerCharacter()->GetIsTurningInPlace())
	{
		GetScaledHeadSocketDeltaRotation(SocketRotation, DeltaTime);
	}
	else
	{
		SocketRotation.Pitch = 0.0f;
		SocketRotation.Roll = 0.0f;
		SocketRotation.Yaw = 0.0f;
	}
	
		GetPlayerCharacter()->GetCamera()->SetWorldRotation(Sway + CentripetalRotation + SocketRotation + PlayerCharacterController->GetPlayerControlRotation());
		GetPlayerCharacter()->GetCamera()->SetWorldRotation(Sway + CentripetalRotation + SocketRotation + PlayerCharacterController->GetPlayerControlRotation());
}

/** Called by UpdateCameraRotation. */
void UPlayerCameraController::GetCameraSwayRotation(FRotator& Rotator)
{
	/** Get the current ground movement type from the PlayerController. */
	if (!GetPlayerCharacter()->GetPlayerMovement()) { return; }
	
	const EPlayerGroundMovementType MovementType {GetPlayerCharacter()->GetPlayerMovement()->GetGroundMovementType()};
	/** Get a oscillation multiplier value according to the ground movement type. */
	float IntensityMultiplier {0.0};
	switch(MovementType)
	{
	case EPlayerGroundMovementType::Idle: IntensityMultiplier = 0.1;
		break;
	case EPlayerGroundMovementType::Walking: IntensityMultiplier = 0.3;
		break;
	case EPlayerGroundMovementType::Sprinting: IntensityMultiplier = 1.65;
		break;
	}
	/** Get a mapped deviation value that scales the shake intensity and speed. Used to introduce some cyclical pseudo-random variance. */
	const double Deviation {FMath::GetMappedRangeValueClamped(FVector2d(-1.0, 1.00), FVector2d(0.75, 1.5),
					UKismetMathLibrary::Cos(UGameplayStatics::GetTimeSeconds(GetWorld()) * 2.4))};
	
	/** Calculate the target shake rotation. */
	const double TargetRollOffset {UKismetMathLibrary::Cos(UGameplayStatics::GetTimeSeconds(GetWorld()) * Deviation) * IntensityMultiplier * Deviation * CameraSwayIntensity};
	
	/** Interpolate between the current camera roll and the target camera roll. */
	CameraShakeRoll = FMath::FInterpTo(CameraShakeRoll, TargetRollOffset, GetWorld()->GetDeltaSeconds(), 3.0);
	
	Rotator.Roll = CameraShakeRoll;
}

/** Called by UpdateCameraRotation. */
void UPlayerCameraController::GetCameraCentripetalRotation(FRotator& Rotator)
{
	const UPlayerMovementComponent* CharacterMovement {GetPlayerCharacter()->GetPlayerMovement()};
	if (!CharacterMovement) { return; }
	if (IsCentripetalRotationSprintOnly && !CharacterMovement->GetIsSprinting()) { return; }
	
	/** When the player is moving laterally while sprinting, we want the camera to lean into that direction. */
	const float LateralVelocityMultiplier {0.002353f * VelocityCentripetalRotation};
	const float SprintMultiplier {IsCentripetalRotationSprintOnly ? CentripetalRotationNonSprintMultiplier : 1.0f};
	const FVector WorldVelocity {GetPlayerCharacter()->GetMovementComponent()->Velocity};
	const FVector LocalVelocity {GetPlayerCharacter()->GetActorTransform().InverseTransformVector(WorldVelocity)};
	const double LateralVelocityRoll {LocalVelocity.Y * LateralVelocityMultiplier * SprintMultiplier};
		
	/** When the player is rotating horizontally while sprinting, we want the camera to lean into that direction. */
	float HorizontalRotationRoll {0.0f};
	if (CharacterMovement->GetIsSprinting())
	{
		HorizontalRotationRoll = FMath::Clamp(PlayerCharacterController->GetHorizontalRotationInput() * RotationCentripetalRotation,
				-MaxCentripetalRotation, MaxCentripetalRotation);
	}
	
	const double TargetRoll {LateralVelocityRoll + HorizontalRotationRoll};
	
	/** Interpolate the roll value. */
	CameraLeanRoll = FMath::FInterpTo(CameraLeanRoll, TargetRoll, GetWorld()->GetDeltaSeconds(), 4.f);
	Rotator.Roll = CameraLeanRoll;
}

void UPlayerCameraController::GetScaledHeadSocketDeltaRotation(FRotator& Rotator, const float DeltaTime)
{
	if (!GetPlayerCharacter()->GetPlayerMovement()) { return; }
	
	
	/** Get the current ground movement type from the PlayerController. */
	const EPlayerGroundMovementType MovementType {GetPlayerCharacter()->GetPlayerMovement()->GetGroundMovementType()};
	
	/** Get a oscillation multiplier value according to the ground movement type. */
	float IntensityMultiplier {0.0};
	if (!GetPlayerCharacter()->GetMovementComponent()->IsFalling())
	{
		switch(MovementType)
		{
		case EPlayerGroundMovementType::Sprinting: IntensityMultiplier = 1.25;
			break;
		default: IntensityMultiplier = 0.5;
			break;
		}
	}
	
	/** Get the delta head socket rotation. */
	FRotator TargetHeadSocketRotation {(GetPlayerCharacter()->GetMesh()->GetSocketTransform("head", RTS_Actor).GetRotation()
		- HeadSocketTransform.GetRotation()) * IntensityMultiplier};

	/** Apply scalars. */
	const float XScalar {static_cast<float>(GetPlayerCharacter()->bIsCrouched ? CrouchedSocketRotationIntensity.X : SocketRotationIntensity.X)};
	const float YScalar	{static_cast<float>(GetPlayerCharacter()->bIsCrouched ? CrouchedSocketRotationIntensity.Y : SocketRotationIntensity.Y)};
	const float ZScalar	{static_cast<float>(GetPlayerCharacter()->bIsCrouched ? CrouchedSocketRotationIntensity.Z : SocketRotationIntensity.Z)};
	
	TargetHeadSocketRotation = FRotator(TargetHeadSocketRotation.Pitch * XScalar, (TargetHeadSocketRotation.Yaw * ZScalar), (TargetHeadSocketRotation.Roll * YScalar));

	/** Interpolate the rotation value to smooth out jerky rotation changes. */
	if (const UWorld* World {GetWorld()})
	{
		InterpolatedHeadSocketRotation = FMath::RInterpTo(InterpolatedHeadSocketRotation, TargetHeadSocketRotation, DeltaTime, 4);
	}
	
	Rotator = InterpolatedHeadSocketRotation;
}

/** Called by TickComponent. */
void UPlayerCameraController::UpdateCameraFieldOfView(UCameraComponent* Camera, const float DeltaTime)
{
	float TargetFOV {DefaultFOV};
	const FVector WorldVelocity {GetPlayerCharacter()->GetMovementComponent()->Velocity};
	const FVector LocalVelocity {GetPlayerCharacter()->GetActorTransform().InverseTransformVector(WorldVelocity)};
	if (LocalVelocity.X > CharacterWalkSpeed * 1.1)
	{
		TargetFOV = FMath::GetMappedRangeValueClamped(FVector2D(CharacterWalkSpeed * 1.1, CharacterSprintSpeed),
													  FVector2D(DefaultFOV, SprintFOV), LocalVelocity.X);
	}

	Camera->FieldOfView = FMath::FInterpTo(Camera->FieldOfView, TargetFOV, DeltaTime, 2.f);
}

void UPlayerCameraController::UpdateCameraVignetteIntensity(UCameraComponent* Camera, const float DeltaTime)
{
	if (GetPlayerCharacter()->GetPlayerMovement())
	{
		const float TargetVignetteIntensity {GetPlayerCharacter()->GetPlayerMovement()->GetIsSprinting()
			? SprintVignetteIntensity : DefaultVignetteIntensity};
		
		if (Camera->PostProcessSettings.VignetteIntensity != TargetVignetteIntensity)
		{
			constexpr float InterpolationSpeed {3};
			Camera->PostProcessSettings.VignetteIntensity =
				FMath::FInterpTo(Camera->PostProcessSettings.VignetteIntensity, TargetVignetteIntensity, DeltaTime, InterpolationSpeed);
		}
	}
}

void UPlayerCameraController::UpdateCameraDepthOfField(UCameraComponent* Camera, const float DeltaTime)
{
	float FocalDistance {GetFocalDistance(Camera)};
	FocalDistance = FMath::Clamp(FocalDistance, MinimumFocalDistance, MaximumFocalDistance);
	
	const float BlurFocus {static_cast<float>(FMath::GetMappedRangeValueClamped
		(FVector2D(MinimumFocalDistance, MaximumFocalDistance),
			FVector2D(MacroBlurFocus,LongShotBlurFocus),FocalDistance))};
	
	const float BlurAmount {static_cast<float>(FMath::GetMappedRangeValueClamped
		(FVector2D(MinimumFocalDistance, MaximumFocalDistance),
			FVector2D(MacroBlurAmount,LongShotBlurAmount),FocalDistance))};

	GetPlayerCharacter()->GetCamera()->PostProcessSettings.DepthOfFieldFocalDistance =
		FMath::FInterpTo(Camera->PostProcessSettings.DepthOfFieldFocalDistance, FocalDistance, DeltaTime, DynamicDofSpeed);
	
	GetPlayerCharacter()->GetCamera()->PostProcessSettings.DepthOfFieldDepthBlurAmount =
		FMath::FInterpTo(Camera->PostProcessSettings.DepthOfFieldDepthBlurAmount, BlurFocus, DeltaTime, DynamicDofSpeed);
	
	GetPlayerCharacter()->GetCamera()->PostProcessSettings.DepthOfFieldDepthBlurRadius =
		FMath::FInterpTo(Camera->PostProcessSettings.DepthOfFieldDepthBlurRadius, BlurAmount, DeltaTime, DynamicDofSpeed);
}

float UPlayerCameraController::GetFocalDistance(UCameraComponent* Camera) const
{
	if (!GetPlayerCharacter())
	{
		return 0.0f;
	}
	
	FVector CameraLocation = Camera->GetComponentLocation();
	FVector ForwardVector = Camera->GetForwardVector();

	constexpr float TraceLength {50000.0f};
	ForwardVector *= TraceLength;
	FVector End = CameraLocation + ForwardVector;

	FHitResult HitResult;
	if (Camera->GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, End, ECollisionChannel::ECC_Visibility))
	{
		return (HitResult.Location - CameraLocation).Size();
	}
	return TraceLength;
}

void UPlayerCameraController::FadeFromBlack(const float Duration)
{
	if (const APlayerController* PlayerController = GetWorld()->GetFirstPlayerController()){
		const FLinearColor FadeColor(0.0f, 0.0f, 0.0f, 1.0f);
		PlayerController->PlayerCameraManager->StartCameraFade(1.0f, 0.0f, Duration, FadeColor);
	}
}






