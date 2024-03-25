// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHPlayerCameraComponent.h"

#include "BHPlayerCharacter.h"
#include "BHCharacterMovementComponent.h"

UBHPlayerCameraComponent::UBHPlayerCameraComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	DynamicVignetteIntensityCurve.GetRichCurve()->AddKey(300.0f, 1.0f);
	DynamicVignetteIntensityCurve.GetRichCurve()->AddKey(450.0f, 1.385);

	CameraQueryParams.OwnerTag = "PlayerCameraComponent";
	CameraQueryParams.TraceTag = "PlayerCameraTrace";
	CameraQueryParams.MobilityType = EQueryMobilityType::Any;
}

void UBHPlayerCameraComponent::PostLoad()
{
	Super::PostLoad();
	
	CameraQueryParams.bTraceComplex = bTraceComplex;
	BasePostProcessSettings = PostProcessSettings;
}

void UBHPlayerCameraComponent::OnRegister()
{
	Super::OnRegister();

	CameraQueryParams.AddIgnoredActor(GetOwner());
}

void UBHPlayerCameraComponent::OnUnregister()
{
	Super::OnUnregister();

	CameraQueryParams.ClearIgnoredActors();
}

void UBHPlayerCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CameraQuery.HitResult.Reset(0.0f, false);
	
	const FVector StartLocation = GetComponentLocation();
	const FVector EndLocation = StartLocation + GetForwardVector() * CameraTraceLength;
	
	const bool bHit = GetWorld()->LineTraceSingleByChannel(
		CameraQuery.HitResult,
		StartLocation,
		EndLocation,
		ECC_Visibility,
		CameraQueryParams
	);
	
	if (bHit)
	{
		CameraQuery.TraceLength = (CameraQuery.HitResult.Location - StartLocation).Size();
	}
	else
	{
		CameraQuery.TraceLength = CameraTraceLength;
	}
	
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

void UBHPlayerCameraComponent::UpdateFOV(const float DeltaTime)
{
	const ABHPlayerCharacter* Character = GetPlayerCharacter();

	float TargetFOV = DefaultFOV;
	const FVector WorldVelocity = Character->GetMovementComponent()->Velocity;
	const FVector LocalVelocity = Character->GetActorTransform().InverseTransformVector(WorldVelocity);

	if (LocalVelocity.X > Character->GetPlayerMovementComponent()->GetMovementSetup().WalkSpeed * 1.1)
	{
		const FBHCharacterSetup& Settings = Character->GetPlayerMovementComponent()->GetMovementSetup();
		TargetFOV = FMath::GetMappedRangeValueClamped(FVector2D(Settings.WalkSpeed * 1.1, Settings.SprintSpeed),
			FVector2D(DefaultFOV, SprintFOV), LocalVelocity.X);
	}

	FieldOfView = FMath::FInterpTo(FieldOfView, TargetFOV, DeltaTime, 2.f);
}

void UBHPlayerCameraComponent::UpdateVignette(const float DeltaTime)
{
	if (GetPlayerCharacter()->GetPlayerMovementComponent())
	{
		const float Speed = GetPlayerCharacter()->GetSpeed();
		const float Scalar = DynamicVignetteIntensityCurve.GetRichCurve()->Eval(Speed);
		const float TargetVignetteIntensity = BasePostProcessSettings.VignetteIntensity * Scalar;
		
		if (PostProcessSettings.VignetteIntensity - TargetVignetteIntensity > UE_KINDA_SMALL_NUMBER)
		{
			PostProcessSettings.VignetteIntensity =
				FMath::FInterpTo(PostProcessSettings.VignetteIntensity, TargetVignetteIntensity,
					DeltaTime, DynamicVignetteSpeed);
		}
	}
}

void UBHPlayerCameraComponent::UpdateDOF(const float DeltaTime)
{
	float FocalDistance = GetFocalDistance();
	FocalDistance = FMath::Clamp(FocalDistance, MinimumFocalDistance, MaximumFocalDistance);
	
	const float BlurFocus = (FMath::GetMappedRangeValueClamped
	(FVector2D(MinimumFocalDistance, MaximumFocalDistance),
	 FVector2D(MacroBlurFocus, LongShotBlurFocus), FocalDistance));

	const float BlurAmount = (FMath::GetMappedRangeValueClamped
	(FVector2D(MinimumFocalDistance, MaximumFocalDistance),
	 FVector2D(MacroBlurAmount, LongShotBlurAmount), FocalDistance));

	PostProcessSettings.DepthOfFieldFocalDistance =
		FMath::FInterpTo(PostProcessSettings.DepthOfFieldFocalDistance, FocalDistance, DeltaTime, DynamicDofSpeed);
	
	PostProcessSettings.DepthOfFieldDepthBlurAmount =
		FMath::FInterpTo(PostProcessSettings.DepthOfFieldDepthBlurAmount, BlurFocus, DeltaTime, DynamicDofSpeed);
	
	PostProcessSettings.DepthOfFieldDepthBlurRadius =
		FMath::FInterpTo(PostProcessSettings.DepthOfFieldDepthBlurRadius, BlurAmount, DeltaTime, DynamicDofSpeed);
}

const FBHCameraQuery& UBHPlayerCameraComponent::GetCameraQuery() const
{
	return CameraQuery;
}

float UBHPlayerCameraComponent::GetFocalDistance() const
{
	return CameraQuery.TraceLength;
}