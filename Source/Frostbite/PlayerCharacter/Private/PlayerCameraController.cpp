// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#include "PlayerCameraController.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterController.h"
#include "PlayerCharacterMovementComponent.h"

#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetMathLibrary.h"

/** Sets default values for this component's properties. */
UPlayerCameraController::UPlayerCameraController()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerCameraController::OnRegister()
{
	Super::OnRegister();
	
	/** Load the configuration asset. If no configuration asset is provided, construct a default instance of the configuration asset instead. */
	if(!Configuration)
	{
		Configuration = ConfigurationAsset.LoadSynchronous();
		if(!Configuration)
		{
			Configuration = NewObject<UPlayerCameraConfiguration>();
			Configuration->AddToRoot();
		}
	}
	
	PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
	if(!PlayerCharacter) {return; }
	HeadSocketTransform = PlayerCharacter->GetMesh()->GetSocketTransform("head", RTS_Actor);
	PlayerCharacter->ReceiveControllerChangedDelegate.AddDynamic(this, &UPlayerCameraController::HandleCharacterControllerChanged);

	/** Apply the camera configuration. */
	if(!Configuration) {return; }
	Configuration->ApplyToCamera(PlayerCharacter->GetCamera());
}

/** Called after the pawn's controller has changed. */
void UPlayerCameraController::HandleCharacterControllerChanged(APawn* Pawn, AController* OldController, AController* NewController)
{
	if(APlayerCharacterController* Controller {Cast<APlayerCharacterController>(NewController)})
	{
		PlayerCharacterController = Controller;
	}
}

/** Called when the game starts. */
void UPlayerCameraController::BeginPlay()
{
	Super::BeginPlay();
	/** Sets the starting color to black so that we can fade in the camera when the player is fully initialized. */
	if(const APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		const FLinearColor Color(0.0f, 0.0f, 0.0f, 1.0f);
		PlayerController->PlayerCameraManager->SetManualCameraFade(1.0f, Color, false);
	}
	
	if (const APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager)
		{
			CameraManager->ViewPitchMax = Configuration->MaximumViewPitch;
			CameraManager->ViewPitchMin = Configuration->MinimumViewPitch;
		}
	}
}


/** Called every frame. */
void UPlayerCameraController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if(PlayerCharacter && PlayerCharacterController)
		if(UCameraComponent* Camera {PlayerCharacter->GetCamera()})
	{
		UpdateCameraRotation(*Camera, DeltaTime); /** Even with camera sway and centripetal rotation disabled, we need to call this function every frame to update the actual orientation of the camera. */
		UpdateCameraLocation(*Camera);
		if(Configuration->IsDynamicFOVEnabled)
		{
			UpdateCameraFieldOfView(*Camera, DeltaTime);
		}
		if(Configuration->IsDynamicVignetteEnabled)
		{
			UpdateCameraVignetteIntensity(*Camera, DeltaTime);
		}
		if(Configuration->IsDynamicDOFEnabled)
		{
			UpdateCameraDepthOfField(*Camera, DeltaTime);
		}
	}
}

// Called by TickComponent.
void UPlayerCameraController::UpdateCameraLocation(UCameraComponent& Camera)
{
	/** Get an alpha value based on the pitch of the camera. We do not want the camera to explicitly follow the head socket if the body of the player isn't visible (e.g. looking down),
	 as this could be perceived as annoying by the user. */ 
	const double PitchAlpha
	{FMath::GetMappedRangeValueClamped(FVector2d(-30.0, -55.0), FVector2d(0.0, 1.0), Camera.GetComponentRotation().Pitch)};
	
	/** Get the delta position of the current head socket location in relation to the default location. This allows us to introduce some socket-bound headbobbing with scalable intensity. */
	const FVector SocketLocation
	{FVector(0, 0,(PlayerCharacter->GetMesh()->GetSocketTransform("head", RTS_Actor).GetLocation() - HeadSocketTransform.GetLocation()).Z * 0.5)};
	
	FVector Result {FVector()};
	/** If the player is looking forward or up, we don't need to perform any additional calculations and can set the relative location to the CameraConfiguration's default value. */
	if(PitchAlpha == 0.0)
	{
		Result = Configuration->CameraOffset + (SocketLocation * !PlayerCharacter->GetIsTurningInPlace());
		
	}
	else
	{
		/** Get the target location if the player is not looking down. */
		const FVector UprightCameraLocation {Configuration->CameraOffset + (SocketLocation * !PlayerCharacter->GetIsTurningInPlace())};
		
		/** Calculate the target location if the player is looking down. */
		const FVector DownwardCameraLocation {PlayerCharacter->GetMesh()->GetSocketTransform("head", RTS_Actor).GetLocation() + FVector(Configuration->CameraOffset.X * 0.625, 0, 0)
		- FVector(0, 0, (PlayerCharacter->GetVelocity().X * 0.02))}; // We lower the camera slightly when the character is moving forward to simulate the body leaning forward.
		
		/** Interpolate between the two target locations depending on PitchAlpha. */
		Result = FMath::Lerp(UprightCameraLocation, DownwardCameraLocation, PitchAlpha); //NOTE: In UE 5.1 using FMath::Lerp() with two FVectors can cause semantic errors, but the code will compile and run just fine.
	}
	/** Rotate the result with the base aim rotation. */
	const FRotator ControlRotation {FRotator(0, PlayerCharacterController->GetControlRotation().Yaw, 0)};
	Result = ControlRotation.RotateVector(Result);
	
	/** Add the world location of the pawn to the result. */
	Result = Result + PlayerCharacter->GetActorLocation();
	
	Camera.SetWorldLocation(Result);
}

/** Called by TickComponent. */
void UPlayerCameraController::UpdateCameraRotation(const UCameraComponent&, const float DeltaTime)
{
	const FRotator Sway {Configuration->IsCameraSwayEnabled ? GetCameraSwayRotation() : FRotator()};
	const FRotator CentripetalRotation {Configuration->IsCentripetalRotationEnabled ? GetCameraCentripetalRotation() : FRotator()};
	FRotator SocketRotation {FRotator()};
	if(!PlayerCharacter->GetIsTurningInPlace())
	{
		SocketRotation = GetScaledHeadSocketDeltaRotation(DeltaTime);
	}
	if(Configuration->IsCameraLagEnabled)
	{
		const float InterpolationSpeed {(1 - Configuration->CameraLagIntensity) * 4 + 6};
		CameraControlRotation = FMath::RInterpTo(CameraControlRotation, PlayerCharacter->GetControlRotation(), DeltaTime, InterpolationSpeed);
		PlayerCharacter->GetCamera()->SetWorldRotation(Sway + CentripetalRotation + SocketRotation + CameraControlRotation);
	}
	else
	{
		PlayerCharacter->GetCamera()->SetWorldRotation(Sway + CentripetalRotation + SocketRotation + PlayerCharacter->GetControlRotation());
	}
	
}

/** Called by UpdateCameraRotation. */
FRotator UPlayerCameraController::GetCameraSwayRotation()
{
	/** Get the current ground movement type from the PlayerController. */
	if(!PlayerCharacter->GetPlayerCharacterMovement()) {return FRotator();}
	
	const EPlayerGroundMovementType MovementType {PlayerCharacter->GetPlayerCharacterMovement()->GetGroundMovementType()};
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
	float Intensity {Configuration->CameraShakeIntensity};
	const double TargetRollOffset {UKismetMathLibrary::Cos(UGameplayStatics::GetTimeSeconds(GetWorld()) * Deviation) * IntensityMultiplier * Deviation * Configuration->CameraShakeIntensity};
	
	/** Interpolate between the current camera roll and the target camera roll. */
	CameraShakeRoll = FMath::FInterpTo(CameraShakeRoll, TargetRollOffset, GetWorld()->GetDeltaSeconds(), 3.0);
	
	/** Return a rotator with the camera roll offset. */
	return FRotator(0, 0, CameraShakeRoll);
}

/** Called by UpdateCameraRotation. */
FRotator UPlayerCameraController::GetCameraCentripetalRotation()
{
	FRotator Rotation {FRotator()};
	double TargetRoll {0.0};
	if(PlayerCharacter->GetPlayerCharacterMovement() && PlayerCharacter->GetPlayerCharacterMovement()->GetIsSprinting())
	{
		/** When the player is moving laterally while sprinting, we want the camera to lean into that direction. */
		const float LateralVelocityMultiplier {0.002353f * Configuration->VelocityCentripetalRotation};
		const FVector WorldVelocity {PlayerCharacter->GetMovementComponent()->Velocity};
		const FVector LocalVelocity {PlayerCharacter->GetActorTransform().InverseTransformVector(WorldVelocity)};
		const double LateralVelocityRoll {LocalVelocity.Y * LateralVelocityMultiplier};
		
		/** When the player is rotating horizontally while sprinting, we want the camera to lean into that direction. */
		const float HorizontalRotationRoll{FMath::Clamp(PlayerCharacterController->GetHorizontalRotationInput() * Configuration->RotationCentripetalRotation,
					-Configuration->MaxCentripetalRotation, Configuration->MaxCentripetalRotation)};

		TargetRoll = LateralVelocityRoll + HorizontalRotationRoll;
	}
	/** Interpolate the roll value. */
	CameraLeanRoll = FMath::FInterpTo(CameraLeanRoll, TargetRoll, GetWorld()->GetDeltaSeconds(), 4.f);
	Rotation = (FRotator(0, 0, CameraLeanRoll));
	return Rotation;
}

FRotator UPlayerCameraController::GetScaledHeadSocketDeltaRotation(const float DeltaTime)
{
	/** Get the current ground movement type from the PlayerController. */
	if(!PlayerCharacter->GetPlayerCharacterMovement()) {return FRotator();};
	const EPlayerGroundMovementType MovementType {PlayerCharacter->GetPlayerCharacterMovement()->GetGroundMovementType()};
	/** Get a oscillation multiplier value according to the ground movement type. */
	float IntensityMultiplier {0.0};
	if(!PlayerCharacter->GetMovementComponent()->IsFalling())
	{
		switch(MovementType)
		{
		case EPlayerGroundMovementType::Sprinting: IntensityMultiplier = 1.25; // Sprinting
			break;
		default: IntensityMultiplier = 0.5; // Miscellaneous
			break;
		}
	}
	/** Get the delta head socket rotation. */
	FRotator TargetHeadSocketRotation {(PlayerCharacter->GetMesh()->GetSocketTransform("head", RTS_Actor).GetRotation()
		- HeadSocketTransform.GetRotation()) * IntensityMultiplier};

	/** Apply scalars. */
	TargetHeadSocketRotation = FRotator(TargetHeadSocketRotation.Pitch, (TargetHeadSocketRotation.Yaw * 0), (TargetHeadSocketRotation.Roll * 1.5));

	/** Interpolate the rotation value to smooth out jerky rotation changes. */
	if(const UWorld* World {GetWorld()})
	{
		InterpolatedHeadSocketRotation = FMath::RInterpTo(InterpolatedHeadSocketRotation, TargetHeadSocketRotation, DeltaTime, 4);
	}
	return InterpolatedHeadSocketRotation;
}

/** Called by TickComponent. */
void UPlayerCameraController::UpdateCameraFieldOfView(UCameraComponent& Camera, const float DeltaTime)
{
	if(const UPlayerCharacterConfiguration* CharacterConfiguration {PlayerCharacter->GetCharacterConfiguration()})
	{
		float TargetFOV {Configuration->DefaultFOV};
		const FVector WorldVelocity {PlayerCharacter->GetMovementComponent()->Velocity};
		const FVector LocalVelocity {PlayerCharacter->GetActorTransform().InverseTransformVector(WorldVelocity)};
		if (LocalVelocity.X > CharacterConfiguration->WalkSpeed * 1.1)
		{
			TargetFOV = FMath::GetMappedRangeValueClamped(FVector2D(CharacterConfiguration->WalkSpeed * 1.1, CharacterConfiguration->SprintSpeed),
						FVector2D(Configuration->DefaultFOV, Configuration->SprintFOV), LocalVelocity.X);
		} 

		Camera.FieldOfView = FMath::FInterpTo(Camera.FieldOfView, TargetFOV, DeltaTime,2.f );
	}
}

void UPlayerCameraController::UpdateCameraVignetteIntensity(UCameraComponent& Camera, const float DeltaTime)
{
	if(PlayerCharacter->GetPlayerCharacterMovement())
	{
		const float TargetVignetteIntensity {PlayerCharacter->GetPlayerCharacterMovement()->GetIsSprinting()
			? Configuration->SprintVignetteIntensity : Configuration->DefaultVignetteIntensity};
		
		if(Camera.PostProcessSettings.VignetteIntensity != TargetVignetteIntensity)
		{
			constexpr float InterpolationSpeed {3};
			Camera.PostProcessSettings.VignetteIntensity =
				FMath::FInterpTo(Camera.PostProcessSettings.VignetteIntensity, TargetVignetteIntensity, DeltaTime, InterpolationSpeed);
		}
	}
}

void UPlayerCameraController::UpdateCameraDepthOfField(const UCameraComponent& Camera, const float DeltaTime)
{
	float FocalDistance {GetFocalDistance(Camera)};
	FocalDistance = FMath::Clamp(FocalDistance, Configuration->MinimumFocalDistance, Configuration->MaximumFocalDistance);
	
	const float BlurFocus {static_cast<float>(FMath::GetMappedRangeValueClamped
		(FVector2D(Configuration->MinimumFocalDistance, Configuration->MaximumFocalDistance),
			FVector2D(Configuration->MacroBlurFocus,Configuration->LongShotBlurFocus),FocalDistance))};
	
	const float BlurAmount {static_cast<float>(FMath::GetMappedRangeValueClamped
		(FVector2D(Configuration->MinimumFocalDistance, Configuration->MaximumFocalDistance),
			FVector2D(Configuration->MacroBlurAmount,Configuration->LongShotBlurAmount),FocalDistance))};

	PlayerCharacter->GetCamera()->PostProcessSettings.DepthOfFieldFocalDistance =
		FMath::FInterpTo(Camera.PostProcessSettings.DepthOfFieldFocalDistance, FocalDistance, DeltaTime, Configuration->DynamicDofSpeed);
	
	PlayerCharacter->GetCamera()->PostProcessSettings.DepthOfFieldDepthBlurAmount = 
		FMath::FInterpTo(Camera.PostProcessSettings.DepthOfFieldDepthBlurAmount, BlurFocus, DeltaTime, Configuration->DynamicDofSpeed);
	
	PlayerCharacter->GetCamera()->PostProcessSettings.DepthOfFieldDepthBlurRadius =
		FMath::FInterpTo(Camera.PostProcessSettings.DepthOfFieldDepthBlurRadius, BlurAmount, DeltaTime, Configuration->DynamicDofSpeed);
}

float UPlayerCameraController::GetFocalDistance(const UCameraComponent& Camera) const
{
	if (!PlayerCharacter)
	{
		return 0.0f;
	}
	
	FVector CameraLocation = Camera.GetComponentLocation();
	FVector ForwardVector = Camera.GetForwardVector();

	constexpr float TraceLength {50000.0f};
	ForwardVector *= TraceLength;
	FVector End = CameraLocation + ForwardVector;

	FHitResult HitResult;
	if (Camera.GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, End, ECollisionChannel::ECC_Visibility))
	{
		return (HitResult.Location - CameraLocation).Size();
	}
	return TraceLength;
}

void UPlayerCameraController::FadeFromBlack(const float Duration)
{
	if(const APlayerController* PlayerController = GetWorld()->GetFirstPlayerController()){
		const FLinearColor FadeColor(0.0f, 0.0f, 0.0f, 1.0f);
		PlayerController->PlayerCameraManager->StartCameraFade(1.0f, 0.0f, Duration, FadeColor);
	}
}






