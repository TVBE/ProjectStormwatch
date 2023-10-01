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
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UPlayerCameraController::OnRegister()
{
	Super::OnRegister();
		
	HeadSocketTransform = GetPlayerCharacter()->GetMesh()->GetSocketTransform("head", RTS_Actor);
}

void UPlayerCameraController::BeginPlay()
{
	Super::BeginPlay();
}

void UPlayerCameraController::OnControllerChanged(APawn* Pawn, AController* NewController, AController* PreviousController)
{
	if (NewController)
	{
		SetComponentTickEnabled(true);

		if (APlayerController * PlayerController {Cast<APlayerController>(NewController)})
		{
			SetCameraPitchLimits(PlayerController->PlayerCameraManager);

			if (!PreviousController)
			{
				const FLinearColor Color(0.0f, 0.0f, 0.0f, 1.0f);
				PlayerController->PlayerCameraManager->SetManualCameraFade(1.0f, Color, false);
			}
		}
	}
	else
	{
		SetComponentTickEnabled(false);
	}
}

void UPlayerCameraController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	UpdateCameraRotation(DeltaTime);
	UpdateCameraLocation(DeltaTime);
	if (IsDynamicVignetteEnabled)
	{
		UpdateVignette(DeltaTime);
	}
	if (IsDynamicFOVEnabled)
	{
		UpdateFOV(DeltaTime);
	}
	if (IsDynamicDOFEnabled)
	{
		UpdateDOF(DeltaTime);
	}
}

void UPlayerCameraController::UpdateCameraLocation(const float DeltaTime)
{	
	FVector TargetLocation {CalculateTargetCameraLocation()};
	const FRotator ControlRotation {GetControlRotation()};
	TargetLocation = ControlRotation.RotateVector(TargetLocation);
	TargetLocation += GetPlayerCharacter()->GetActorLocation();
	
	GetCamera()->SetWorldLocation(TargetLocation);
}

FVector UPlayerCameraController::CalculateTargetCameraLocation() const
{
	const float Alpha {GetViewPitchAlpha()};
	const FVector SocketZOffset {GetHeadSocketZOffset()};

	const APlayerCharacter* Character {GetPlayerCharacter()};

	FVector MinAlphaLocation {CameraOffset + (SocketZOffset * !Character->GetIsTurningInPlace())};

	FVector Result;
	if (Alpha == 0.0f)
	{
		return MinAlphaLocation;
	}
	else
	{
		FVector ForwardVector {Character->GetActorForwardVector()};
		float ForwardVelocity {FVector::DotProduct(Character->GetVelocity(), ForwardVector)};

		FVector HeadSocketLocation {Character->GetMesh()->GetSocketTransform("head", RTS_Actor).GetLocation()};
		FVector DownwardAdjustment {FVector(CameraOffset.X * 0.625, 0, 0) - FVector(0, 0, (ForwardVelocity * 0.02))};
		FVector MaxAlphaLocation {HeadSocketLocation + DownwardAdjustment};

		return FMath::Lerp(MinAlphaLocation, MaxAlphaLocation, Alpha);
	}
}

void UPlayerCameraController::UpdateCameraRotation(const float DeltaTime)
{
	FRotator TargetRotation {GetPlayerCharacterController()->GetPlayerControlRotation()};
	if (!GetPlayerCharacter()->GetIsTurningInPlace())
	{
		AddScaledHeadSocketDeltaRotation(TargetRotation, DeltaTime);
	}
	if (IsCameraSwayEnabled)
	{
		AddCameraSwayRotation(TargetRotation, DeltaTime);
	}
	if (IsCentripetalRotationEnabled)
	{
		AddCameraCentripetalRotation(TargetRotation, DeltaTime);
	}
	GetCamera()->SetWorldRotation(TargetRotation);
}

void UPlayerCameraController::AddCameraSwayRotation(FRotator& Rotator, const float DeltaTime)
{	
	float IntensityMultiplier {0.0f};
	const EPlayerGroundMovementType MovementType {GetPlayerCharacter()->GetPlayerMovement()->GetGroundMovementType()};
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

float UPlayerCameraController::GetCameraSwayIntensity() const
{
	return 0.0f;
}

/** Called by UpdateCameraRotation. */
void UPlayerCameraController::AddCameraCentripetalRotation(FRotator& Rotator, const float DeltaTime)
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
		HorizontalRotationRoll = FMath::Clamp(GetPlayerCharacterController()->GetHorizontalRotationInput() * RotationCentripetalRotation,
				-MaxCentripetalRotation, MaxCentripetalRotation);
	}
	
	const double TargetRoll {LateralVelocityRoll + HorizontalRotationRoll};
	
	/** Interpolate the roll value. */
	CameraLeanRoll = FMath::FInterpTo(CameraLeanRoll, TargetRoll, GetWorld()->GetDeltaSeconds(), 4.f);
	Rotator.Roll = CameraLeanRoll;
}

void UPlayerCameraController::AddScaledHeadSocketDeltaRotation(FRotator& Rotator, const float DeltaTime)
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
void UPlayerCameraController::UpdateFOV(const float DeltaTime)
{
	APlayerCharacter* Character {GetPlayerCharacter()};

	float TargetFOV {DefaultFOV};

	const FVector WorldVelocity {Character->GetMovementComponent()->Velocity};
	const FVector LocalVelocity {Character->GetActorTransform().InverseTransformVector(WorldVelocity)};

	if (LocalVelocity.X > Character->GetWalkSpeed() * 1.1)
	{
		TargetFOV = FMath::GetMappedRangeValueClamped(FVector2D(Character->GetWalkSpeed() * 1.1, Character->GetSprintSpeed()),
													  FVector2D(DefaultFOV, SprintFOV), LocalVelocity.X);
	}

	Camera->FieldOfView = FMath::FInterpTo(Camera->FieldOfView, TargetFOV, DeltaTime, 2.f);
}

void UPlayerCameraController::UpdateVignette(const float DeltaTime)
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

void UPlayerCameraController::UpdateDOF(const float DeltaTime)
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

float UPlayerCameraController::GetFocalDistance() const
{
	UCameraComponent* Camera {GetPlayerCharacter()->GetCamera()};
	
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

float UPlayerCameraController::GetViewPitchAlpha() const
{
	return FMath::GetMappedRangeValueClamped(FVector2d(-30.0, -55.0),
											 FVector2d(0.0, 1.0),
											 GetCamera()->GetComponentRotation().Pitch);
}

FVector UPlayerCameraController::GetHeadSocketZOffset() const
{
	const FVector CurrentHeadSocketLocation = GetPlayerCharacter()->GetMesh()->GetSocketTransform("head", RTS_Actor).GetLocation();
	float ZOffset = (CurrentHeadSocketLocation - HeadSocketTransform.GetLocation()).Z * 0.5;
	return FVector(0, 0, ZOffset);
}

UCameraComponent* UPlayerCameraController::GetCamera() const
{
	return GetPlayerCharacter()->GetCamera();
}

FRotator UPlayerCameraController::GetControlRotation() const
{
	return FRotator(0, GetPlayerCharacterController()->GetPlayerControlRotation().Yaw, 0);
}

void UPlayerCameraController::FadeFromBlack(const float Duration)
{
	if (const APlayerController* PlayerController = GetWorld()->GetFirstPlayerController()){
		const FLinearColor FadeColor(0.0f, 0.0f, 0.0f, 1.0f);
		PlayerController->PlayerCameraManager->StartCameraFade(1.0f, 0.0f, Duration, FadeColor);
	}
}

void UPlayerCameraController::SetCameraPitchLimits(TObjectPtr<APlayerCameraManager> CameraManager)
{
	if (CameraManager.IsNull()) { return; }

	CameraManager->ViewPitchMax = MaximumViewPitch;
	CameraManager->ViewPitchMin = MinimumViewPitch;
}






