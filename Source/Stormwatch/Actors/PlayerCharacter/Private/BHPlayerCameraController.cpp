// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHPlayerCameraController.h"

#include "BHPlayerCameraComponent.h"
#include "BHPlayerCharacter.h"
#include "BHPlayerController.h"
#include "BHPlayerMovementComponent.h"

#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"

#if WITH_EDITOR
#include "Editor/UnrealEd/Public/Editor.h"
#endif

UBHPlayerCameraController::UBHPlayerCameraController()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UBHPlayerCameraController::OnRegister()
{
	Super::OnRegister();
		
	HeadSocketTransform = GetPlayerCharacter()->GetMesh()->GetSocketTransform("head", RTS_Actor);
	ComponentLocationOrigin = GetComponentLocation();
}

void UBHPlayerCameraController::BeginPlay()
{
	Super::BeginPlay();
}

void UBHPlayerCameraController::OnControllerChanged(APawn* Pawn, AController* NewController, AController* PreviousController)
{
	if (NewController)
	{
		SetComponentTickEnabled(true);

		if (APlayerController * PlayerController = Cast<APlayerController>(NewController))
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

void UBHPlayerCameraController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	UpdateRotation(DeltaTime);
	UpdateLocation(DeltaTime);
	if (bDynamicVignetteEnabled)
	{
		UpdateVignette(DeltaTime);
	}
	if (bDynamicFOVEnabled)
	{
		UpdateFOV(DeltaTime);
	}
	if (bDynamicDOFEnabled)
	{
		UpdateDOF(DeltaTime);
	}
}

void UBHPlayerCameraController::UpdateLocation(const float DeltaTime)
{	
	FVector TargetLocation = CalculateTargetLocation();
	const FRotator ControlRotation = GetControlRotation();
	TargetLocation = ControlRotation.RotateVector(TargetLocation);
	TargetLocation += GetPlayerCharacter()->GetActorLocation();
	
	SetWorldLocation(TargetLocation);
}

FVector UBHPlayerCameraController::CalculateTargetLocation() const
{
	const ABHPlayerCharacter* Character = GetPlayerCharacter();

	const float Alpha = GetViewPitchAlpha();
	const FVector SocketZOffset = GetHeadSocketZOffset();
	const FVector MinAlphaLocation = ComponentLocationOrigin + (SocketZOffset * !Character->GetIsTurningInPlace());
	
	if (Alpha == 0.0f)
	{
		return MinAlphaLocation;
	}
	
	const FVector ForwardVector = Character->GetActorForwardVector();
	const float ForwardVelocity = FVector::DotProduct(Character->GetVelocity(), ForwardVector);

	const FVector HeadSocketLocation = Character->GetMesh()->GetSocketTransform("head", RTS_Actor).GetLocation();
	const FVector DownwardAdjustment = FVector(ComponentLocationOrigin.X * 0.625, 0, 0) - FVector(0, 0, (ForwardVelocity * 0.02));
	const FVector MaxAlphaLocation = HeadSocketLocation + DownwardAdjustment;

	return FMath::Lerp(MinAlphaLocation, MaxAlphaLocation, Alpha);
}

void UBHPlayerCameraController::UpdateRotation(const float DeltaTime)
{
	FRotator TargetRotation = GetPlayerCharacterController()->GetPlayerControlRotation();
	if (!GetPlayerCharacter()->GetIsTurningInPlace())
	{
		AddScaledHeadSocketDeltaRotation(TargetRotation, DeltaTime);
	}
	if (bCameraSwayEnabled)
	{
		AddCameraSwayRotation(TargetRotation, DeltaTime);
	}
	if (bCentripetalRotationEnabled)
	{
		AddCameraCentripetalRotation(TargetRotation, DeltaTime);
	}
	GetCamera()->SetWorldRotation(TargetRotation);
}

void UBHPlayerCameraController::AddCameraSwayRotation(FRotator& Rotator, const float DeltaTime)
{	
	const double Deviation = FMath::GetMappedRangeValueClamped(
		FVector2d(-1.0, 1.00),
		FVector2d(0.75, 1.5),
		UKismetMathLibrary::Cos(UGameplayStatics::GetTimeSeconds(GetWorld()) * 2.4)
	);

	const double TargetRollOffset = UKismetMathLibrary::Cos(UGameplayStatics::GetTimeSeconds(GetWorld()) 
															* Deviation) * GetCameraSwayIntensity() * Deviation;

	CameraShakeRoll = FMath::FInterpTo(CameraShakeRoll, TargetRollOffset, GetWorld()->GetDeltaSeconds(),
									   3.0);

	Rotator.Roll = CameraShakeRoll;
}

float UBHPlayerCameraController::GetCameraSwayIntensity() const
{
	float Multiplier = 0.0f;

	switch (GetPlayerCharacter()->GetPlayerMovementComponent()->GetGroundMovementType())
	{
	case EBHPlayerGroundMovementType::Idle:
	Multiplier = 0.1;
	break;
	case EBHPlayerGroundMovementType::Walking:
	Multiplier = 0.3;
	break;
	case EBHPlayerGroundMovementType::Sprinting:
	Multiplier = 1.65;
	break;
	}

	Multiplier *= CameraSwayIntensity;
	return Multiplier;
}

void UBHPlayerCameraController::AddCameraCentripetalRotation(FRotator& Rotator, const float DeltaTime)
{
	const UBHPlayerMovementComponent* CharacterMovement = GetPlayerCharacter()->GetPlayerMovementComponent();
	if (!CharacterMovement) { return; }
	if (bCentripetalRotationSprintOnly && !CharacterMovement->GetIsSprinting()) { return; }
	
	const float LateralVelocityMultiplier = 0.002353f * VelocityCentripetalRotation;
	const float SprintMultiplier = bCentripetalRotationSprintOnly ? CentripetalRotationNonSprintMultiplier : 1.0f;
	const FVector WorldVelocity = GetPlayerCharacter()->GetMovementComponent()->Velocity;
	const FVector LocalVelocity = GetPlayerCharacter()->GetActorTransform().InverseTransformVector(WorldVelocity);
	const double LateralVelocityRoll = LocalVelocity.Y * LateralVelocityMultiplier * SprintMultiplier;
		
	float HorizontalRotationRoll = 0.0f;
	if (CharacterMovement->GetIsSprinting())
	{
		HorizontalRotationRoll = FMath::Clamp(GetPlayerCharacterController()->GetHorizontalRotationInput() 
											  * RotationCentripetalRotation,-MaxCentripetalRotation, MaxCentripetalRotation);
	}
	
	const double TargetRoll = LateralVelocityRoll + HorizontalRotationRoll;
	
	/** Interpolate the roll value. */
	CameraLeanRoll = FMath::FInterpTo(CameraLeanRoll, TargetRoll, GetWorld()->GetDeltaSeconds(), 4.f);
	Rotator.Roll = CameraLeanRoll;
}

void UBHPlayerCameraController::AddScaledHeadSocketDeltaRotation(FRotator& Rotator, const float DeltaTime)
{	
	const EBHPlayerGroundMovementType MovementType = GetPlayerCharacter()->GetPlayerMovementComponent()->GetGroundMovementType();
	
	float IntensityMultiplier = 0.0;
	if (!GetPlayerCharacter()->GetMovementComponent()->IsFalling())
	{
		switch(MovementType)
		{
		case EBHPlayerGroundMovementType::Sprinting: IntensityMultiplier = 1.25;
			break;
		default: IntensityMultiplier = 0.5;
			break;
		}
	}
	
	FRotator TargetHeadSocketRotation {(GetPlayerCharacter()->GetMesh()->GetSocketTransform("head", RTS_Actor).GetRotation()
		- HeadSocketTransform.GetRotation()) * IntensityMultiplier};
	
	TargetHeadSocketRotation = FRotator(
		TargetHeadSocketRotation.Pitch * GetPlayerCharacter()->bIsCrouched ? CrouchedSocketRotationIntensity.X : SocketRotationIntensity.X,
		TargetHeadSocketRotation.Yaw * GetPlayerCharacter()->bIsCrouched ? CrouchedSocketRotationIntensity.Z : SocketRotationIntensity.Z,
		TargetHeadSocketRotation.Roll * GetPlayerCharacter()->bIsCrouched ? CrouchedSocketRotationIntensity.Y : SocketRotationIntensity.Y
	);

	if (const UWorld* World = GetWorld())
	{
		InterpolatedHeadSocketRotation = FMath::RInterpTo(InterpolatedHeadSocketRotation, TargetHeadSocketRotation, DeltaTime, 4);
	}
	
	Rotator = InterpolatedHeadSocketRotation;
}

void UBHPlayerCameraController::UpdateFOV(const float DeltaTime)
{
	ABHPlayerCharacter* Character = GetPlayerCharacter();
	UBHPlayerCameraComponent* Camera = Character->GetCamera();

	float TargetFOV = DefaultFOV;
	const FVector WorldVelocity = Character->GetMovementComponent()->Velocity;
	const FVector LocalVelocity = Character->GetActorTransform().InverseTransformVector(WorldVelocity);

	if (LocalVelocity.X > Character->GetPlayerMovementComponent()->GetSettings().WalkSpeed * 1.1)
	{
		const FBHPlayerCharacterMovementSettings& Settings = Character->GetPlayerMovementComponent()->GetSettings();
		TargetFOV = FMath::GetMappedRangeValueClamped(FVector2D(Settings.WalkSpeed * 1.1, Settings.SprintSpeed),
			FVector2D(DefaultFOV, SprintFOV), LocalVelocity.X);
	}

	Camera->FieldOfView = FMath::FInterpTo(Camera->FieldOfView, TargetFOV, DeltaTime, 2.f);
}

void UBHPlayerCameraController::UpdateVignette(const float DeltaTime)
{
	if (GetPlayerCharacter()->GetPlayerMovementComponent())
	{
		const float TargetVignetteIntensity {GetPlayerCharacter()->GetPlayerMovementComponent()->GetIsSprinting()
			? SprintVignetteIntensity : DefaultVignetteIntensity};
		
		if (Camera->PostProcessSettings.VignetteIntensity != TargetVignetteIntensity)
		{
			constexpr float InterpolationSpeed = 3;
			Camera->PostProcessSettings.VignetteIntensity =
				FMath::FInterpTo(Camera->PostProcessSettings.VignetteIntensity, TargetVignetteIntensity, DeltaTime, InterpolationSpeed);
		}
	}
}

void UBHPlayerCameraController::UpdateDOF(const float DeltaTime)
{
	UBHPlayerCameraComponent* Camera = GetPlayerCharacter()->GetCamera();

	float FocalDistance = GetFocalDistance();
	FocalDistance = FMath::Clamp(FocalDistance, MinimumFocalDistance, MaximumFocalDistance);
	
	const float BlurFocus = (FMath::GetMappedRangeValueClamped
	(FVector2D(MinimumFocalDistance, MaximumFocalDistance),
	 FVector2D(MacroBlurFocus, LongShotBlurFocus), FocalDistance));

	const float BlurAmount = (FMath::GetMappedRangeValueClamped
	(FVector2D(MinimumFocalDistance, MaximumFocalDistance),
	 FVector2D(MacroBlurAmount, LongShotBlurAmount), FocalDistance));

	Camera->PostProcessSettings.DepthOfFieldFocalDistance =
		FMath::FInterpTo(Camera->PostProcessSettings.DepthOfFieldFocalDistance, FocalDistance, DeltaTime, DynamicDofSpeed);
	
	Camera->PostProcessSettings.DepthOfFieldDepthBlurAmount =
		FMath::FInterpTo(Camera->PostProcessSettings.DepthOfFieldDepthBlurAmount, BlurFocus, DeltaTime, DynamicDofSpeed);
	
	Camera->PostProcessSettings.DepthOfFieldDepthBlurRadius =
		FMath::FInterpTo(Camera->PostProcessSettings.DepthOfFieldDepthBlurRadius, BlurAmount, DeltaTime, DynamicDofSpeed);
}

float UBHPlayerCameraController::GetFocalDistance() const
{
	UBHPlayerCameraComponent* Camera = GetPlayerCharacter()->GetCamera();
	
	FVector CameraLocation = Camera->GetComponentLocation();
	FVector ForwardVector = Camera->GetForwardVector();

	constexpr float TraceLength = 50000.0f;
	ForwardVector *= TraceLength;
	FVector End = CameraLocation + ForwardVector;

	FHitResult HitResult;
	if (Camera->GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, End, ECollisionChannel::ECC_Visibility))
	{
		return (HitResult.Location - CameraLocation).Size();
	}
	return TraceLength;
}

float UBHPlayerCameraController::GetViewPitchAlpha() const
{
	return FMath::GetMappedRangeValueClamped(FVector2d(-30.0, -55.0),
											 FVector2d(0.0, 1.0),
											 GetCamera()->GetComponentRotation().Pitch);
}

FVector UBHPlayerCameraController::GetHeadSocketZOffset() const
{
	const FVector CurrentHeadSocketLocation = GetPlayerCharacter()->GetMesh()->GetSocketTransform("head", RTS_Actor).GetLocation();
	float ZOffset = (CurrentHeadSocketLocation - HeadSocketTransform.GetLocation()).Z * 0.5;
	return FVector(0, 0, ZOffset);
}

UBHPlayerCameraComponent* UBHPlayerCameraController::GetCamera() const
{
	return GetPlayerCharacter()->GetCamera();
}

FRotator UBHPlayerCameraController::GetControlRotation() const
{
	return FRotator(0, GetPlayerCharacterController()->GetPlayerControlRotation().Yaw, 0);
}

void UBHPlayerCameraController::FadeFromBlack(const float Duration)
{
	if (const APlayerController* PlayerController = GetWorld()->GetFirstPlayerController()){
		const FLinearColor FadeColor(0.0f, 0.0f, 0.0f, 1.0f);
		PlayerController->PlayerCameraManager->StartCameraFade(1.0f, 0.0f, Duration, FadeColor);
	}
}

void UBHPlayerCameraController::SetCameraPitchLimits(TObjectPtr<APlayerCameraManager> CameraManager)
{
	if (CameraManager.IsNull()) { return; }

	CameraManager->ViewPitchMax = MaximumViewPitch;
	CameraManager->ViewPitchMin = MinimumViewPitch;
}






