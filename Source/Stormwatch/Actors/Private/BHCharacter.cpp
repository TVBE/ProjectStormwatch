// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "Stormwatch/Actors/Public/BHCharacter.h"

#include "Components/CapsuleComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogBHCharacter, Log, All)

ABHCharacter::ABHCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABHCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Speed = GetVelocity().Length();
}

float ABHCharacter::GetClearanceAbovePawn(float TraceLength) const
{
	const FVector Start = GetActorLocation();
	const FVector End = Start + FVector(0.f, 0.f, TraceLength);
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams))
	{
		// We subtract the capsule collision half height as this is the generally the distance between the center of
		// the SkeletalMesh and the top of the head.
		return HitResult.Distance - GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	}
	
	return TraceLength;
}

float ABHCharacter::GetSpeed() const
{
	return Speed;
}

