// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHPlayerCameraSocketComponent.h"

#include "BHPlayerCameraComponent.h"
#include "BHPlayerCharacter.h"
#include "BHPlayerController.h"
#include "BHPlayerMovementComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"

DEFINE_LOG_CATEGORY_STATIC(LogBHCameraSocketComponent, Log, All);

UBHPlayerCameraSocketComponent::UBHPlayerCameraSocketComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	CameraSwayIntensityCurve.EditorCurveData.AddKey(0.0f, 0.1f);
	CameraSwayIntensityCurve.EditorCurveData.AddKey(300.0f, 0.3f);
	CameraSwayIntensityCurve.EditorCurveData.AddKey(450.0f, 1.65f);

	SocketSwayIntensityCurve.EditorCurveData.AddKey(300.0f, 0.5f);
	SocketSwayIntensityCurve.EditorCurveData.AddKey(450.0f, 1.25f);

	RotationDrivenCentripetalRollIntensityCurve.EditorCurveData.AddKey(300.0f, 0.0f);
	RotationDrivenCentripetalRollIntensityCurve.EditorCurveData.AddKey(450.0f, 1.0f);

	MovementDrivenCentripetalRollIntensityCurve.EditorCurveData.AddKey(300.0f, 1.0f);
	MovementDrivenCentripetalRollIntensityCurve.EditorCurveData.AddKey(450.0f, 1.25f);
}

void UBHPlayerCameraSocketComponent::OnRegister()
{
	Super::OnRegister();
		
	RelativeTransform = GetComponentTransform();

	if (GetOwner())
	{
		// PrimaryComponentTick.AddPrerequisite(GetOwner())
	}
}

void UBHPlayerCameraSocketComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBHPlayerCameraSocketComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!GetPlayerCharacterController())
	{
		return;
	}
	
	UpdateRotation(DeltaTime);
	UpdateLocation(DeltaTime);
}

void UBHPlayerCameraSocketComponent::UpdateLocation(float DeltaTime)
{	
	FVector TargetLocation = CalculateTargetLocation();
	const FRotator ControlRotation = GetControlRotation();
	TargetLocation = ControlRotation.RotateVector(TargetLocation);
	TargetLocation += GetPlayerCharacter()->GetActorLocation();
	
	SetWorldLocation(TargetLocation);
}

FVector UBHPlayerCameraSocketComponent::CalculateTargetLocation() const
{
	const ABHPlayerCharacter* Character = GetPlayerCharacter();

	const float Alpha = GetViewPitchAlpha();
	const FVector SocketZOffset = GetHeadSocketZOffset();
	const FVector MinAlphaLocation = RelativeTransform.GetLocation() + (SocketZOffset * !Character->GetIsTurningInPlace());
	
	if (Alpha == 0.0f)
	{
		return MinAlphaLocation;
	}
	
	const FVector ForwardVector = Character->GetActorForwardVector();
	const float ForwardVelocity = FVector::DotProduct(Character->GetVelocity(), ForwardVector);

	const FVector HeadSocketLocation = Character->GetMesh()->GetSocketTransform("head", RTS_Actor).GetLocation();
	const FVector DownwardAdjustment = FVector(RelativeTransform.GetLocation().X * 0.625, 0, 0) - FVector(0, 0, (ForwardVelocity * 0.02));
	const FVector MaxAlphaLocation = HeadSocketLocation + DownwardAdjustment;

	return FMath::Lerp(MinAlphaLocation, MaxAlphaLocation, Alpha);
}

void UBHPlayerCameraSocketComponent::UpdateRotation(const float DeltaTime)
{
	FRotator TargetRotation = FMath::Clamp(MinimumViewPitch, MaximumViewPitch,
		GetPlayerCharacterController()->GetPlayerControlRotation());
	
	if (bSocketSwayEnabled && !SocketName.IsNone() && !GetPlayerCharacter()->GetIsTurningInPlace())
	{
		AddSocketSway(TargetRotation, DeltaTime);
	}
	if (bCameraSwayEnabled)
	{
		AddCameraSway(TargetRotation, DeltaTime);
	}
	if (bCentripetalRollEnabled)
	{
		AddCentripetalRoll(TargetRotation, DeltaTime);
	}
	SetWorldRotation(TargetRotation);
}

void UBHPlayerCameraSocketComponent::AddCameraSway(FRotator& Rotator, const float DeltaTime)
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

float UBHPlayerCameraSocketComponent::GetCameraSwayIntensity() const
{
	const float Speed = GetPlayerCharacter()->GetSpeed();
	const float Scalar = CameraSwayIntensityCurve.GetRichCurveConst()->Eval(Speed);
	return CameraSwayIntensity * Scalar;
}

void UBHPlayerCameraSocketComponent::AddCentripetalRoll(FRotator& Rotator, float DeltaTime)
{
	const UBHPlayerMovementComponent* CharacterMovement = GetPlayerCharacter()->GetPlayerMovementComponent();
	const float Speed = GetPlayerCharacter()->GetSpeed();

	const float RotationScalar = RotationDrivenCentripetalRollIntensityCurve.GetRichCurve()->Eval(Speed);
	const float MovementScalar = MovementDrivenCentripetalRollIntensityCurve.GetRichCurve()->Eval(Speed);

	float TargetRoll = 0.0f;

	if (RotationScalar > UE_SMALL_NUMBER)
	{
		TargetRoll += FMath::Clamp(GetPlayerCharacterController()->GetHorizontalRotationInput()
			* RotationScalar,-MaxCentripetalRoll, MaxCentripetalRoll);
	}
	if (MovementScalar > UE_SMALL_NUMBER)
	{
		constexpr float Scalar = 0.002353f; // Magic number.
		const FVector LocalVelocity = GetPlayerCharacter()->GetActorTransform().InverseTransformVector(CharacterMovement->Velocity);
		TargetRoll += LocalVelocity.Y * MovementScalar * Scalar; 
	}
	
	CameraLeanRoll = FMath::FInterpTo(CameraLeanRoll, TargetRoll, DeltaTime, 4.f);
	Rotator.Roll = CameraLeanRoll;
}

void UBHPlayerCameraSocketComponent::AddSocketSway(FRotator& Rotator, const float DeltaTime)
{	
	const float Speed = GetPlayerCharacter()->GetSpeed();
	
	const float Scalar = SocketSwayIntensityCurve.GetRichCurve()->Eval(Speed);
	Scalar *= GetPlayerCharacter()->GetPlayerMovementComponent()->IsCrouching() ? CrouchedSocketSwayIntensity : 1.0f;
	
	const FRotator TargetHeadSocketRotation {(GetPlayerCharacter()->GetMesh()->GetSocketTransform("head", RTS_Actor).GetRotation()
		- RelativeTransform.GetRotation()) * Scalar};
	
	InterpolatedHeadSocketRotation = FMath::RInterpTo(InterpolatedHeadSocketRotation, TargetHeadSocketRotation,
		DeltaTime, SocketSwaySpeed);
	
	Rotator = InterpolatedHeadSocketRotation;
}

float UBHPlayerCameraSocketComponent::GetViewPitchAlpha() const
{
	return FMath::GetMappedRangeValueClamped(SocketLockPitchTransitionRange, FVector2D(0.0, 1.0),
		GetPlayerCharacter()->GetCamera()->GetComponentRotation().Pitch);
}

FVector UBHPlayerCameraSocketComponent::GetHeadSocketZOffset() const
{
	const FVector CurrentHeadSocketLocation = GetPlayerCharacter()->GetMesh()->GetSocketTransform("head", RTS_Actor).GetLocation();
	const float ZOffset = (CurrentHeadSocketLocation - RelativeTransform.GetLocation()).Z * 0.5;
	return FVector(0, 0, ZOffset);
}

FRotator UBHPlayerCameraSocketComponent::GetControlRotation() const
{
	return FRotator(0, GetPlayerCharacterController()->GetPlayerControlRotation().Yaw, 0);
}