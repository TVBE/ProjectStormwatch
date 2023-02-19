// Copyright 2023 Barrelhouse


#include "SurfaceTrackerComponent.h"


// Sets default values for this component's properties
USurfaceTrackerComponent::USurfaceTrackerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void USurfaceTrackerComponent::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void USurfaceTrackerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if(!GetOwner() || !GetWorld())
	{
		return;
	}
	
	const FVector ActorLocation {GetOwner()->GetActorLocation()};
	const float DistanceToSurface {GetDistanceToSurface(GetWorld(), GetComponentLocation(), TrackingLength)};

	float TargetZ;
	if(abs(DistanceToSurface) > DeviationRange)
	{
		TargetZ = GetOwner()->GetActorLocation().Z;
	}
	else
	{
		TargetZ = GetComponentLocation().Z - DistanceToSurface + TrackingHeight;
	}
	TargetZ -= GetOwner()->GetActorLocation().Z;

	const FVector TargetLocation {FVector(GetRelativeLocation().X, GetRelativeLocation().X, TargetZ)};
	SetRelativeLocation(FMath::VInterpTo(GetRelativeLocation(), TargetLocation, GetWorld()->DeltaTimeSeconds, InterpolationSpeed));

#if WITH_EDITOR
	if(EnableDebugVisualisation)
	{
		DrawDebugVisualisation();
	}
#endif
}

float USurfaceTrackerComponent::GetDistanceToSurface(const UWorld* World, const FVector Location, const float TraceLength)
{
	if(!World)
	{
		return TraceLength;
	}
	
	FHitResult HitResult;
	FVector StartLocation {Location};
	FVector EndLocation {Location - FVector(0.f, 0.f, TraceLength)};
	FCollisionQueryParams QueryParams {FCollisionQueryParams::DefaultQueryParam};
	QueryParams.bTraceComplex = false;

	if (World->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_WorldStatic, QueryParams))
	{
		return (HitResult.Location - StartLocation).Size();
	}

	return TraceLength;
}

void USurfaceTrackerComponent::DrawDebugVisualisation()
{
	
	DrawDebugSphere(GetWorld(), GetComponentLocation(), 100.f, 32, FColor::White, false, 0.0f, 0, 1.f);
}

