// Copyright 2023 Barrelhouse


#include "FrostbiteUtilities.h"
#include "PlayerCharacter/FootstepData.h"

FFootstepData UFrostbiteUtilities::GetFootstepData(const UWorld* World, const AActor* Actor, const float TraceLength)
{
	FFootstepData FootstepData {FFootstepData()};
	if(Actor)
	{
		FVector Location {Actor->GetActorLocation()};
		FootstepData.Location = Location;
		FootstepData.Velocity = Actor->GetVelocity().Length();
		
		FHitResult HitResult;
		FVector TraceStart = Location;
		FVector TraceEnd = Location - FVector(0, 0, TraceLength);
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(Actor);
		Params.bTraceComplex = false;
		Params.bReturnPhysicalMaterial = true;

		if (World && World->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, Params))
		{
			FootstepData.Object = HitResult.GetActor();
			FootstepData.PhysicalMaterial = HitResult.PhysMaterial.Get();
			FootstepData.Location = Location;
		}
	}
	return FootstepData;
}
