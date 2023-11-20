// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHPlayerCameraComponent.h"

void FBHCameraTraceData::Reset()
{
	HitResult.Reset();
	TraceDistance = std::numeric_limits<float>::quiet_NaN();
}

UBHPlayerCameraComponent::UBHPlayerCameraComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UBHPlayerCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	DoCameraTrace();
}

const FBHCameraTraceData& UBHPlayerCameraComponent::GetCameraTraceData()
{
	if (!TraceData.bValid)
	{
		DoCameraTrace();
	}
	return TraceData;
}

void UBHPlayerCameraComponent::DoCameraTrace()
{
	TraceData.Reset();
	
	constexpr float CameraTraceLength = 100000.0f;
	const FVector StartLocation = GetComponentLocation();
	const FVector EndLocation = StartLocation + GetForwardVector() * CameraTraceLength;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());
	
	const bool bHit = GetWorld()->LineTraceSingleByChannel(
		TraceData.HitResult,
		StartLocation,
		EndLocation,
		ECC_Visibility,
		QueryParams
	);
	
	TraceData.bValid = bHit;
	if (bHit)
	{
		TraceData.TraceDistance = (TraceData.HitResult.Location - StartLocation).Size();
	}
	else
	{
		TraceData.TraceDistance = CameraTraceLength;
	}
}
