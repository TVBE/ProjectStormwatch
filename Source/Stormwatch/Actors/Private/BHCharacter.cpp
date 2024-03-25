// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "Stormwatch/Actors/Public/BHCharacter.h"

#include "BHCharacterMovementComponent.h"
#include "PropertyCustomizationHelpers.h"
#include "Components/CapsuleComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogBHCharacter, Log, All)

ABHCharacter::ABHCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

ABHCharacter::ABHCharacter(const FObjectInitializer& ObjectInitializer)
: Super(
	ObjectInitializer
	.SetDefaultSubobjectClass<UBHCharacterMovementComponent>(CharacterMovementComponentName)
)
{
}

void ABHCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Speed = GetVelocity().Length();
}

void ABHCharacter::Jump()
{
	Super::Jump();
}

void ABHCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
}

void ABHCharacter::Crouch(bool bClientSimulation)
{
	if (!CanCrouch())
	{
		return;
	}
	Super::Crouch(bClientSimulation);
}

void ABHCharacter::UnCrouch(bool bClientSimulation)
{
	if (!CanUncrouch())
	{
		return;
	}
	Super::UnCrouch(bClientSimulation);
}

bool ABHCharacter::CanCrouch() const
{
	return Setup.bCrouchingEnabled;
}

bool ABHCharacter::CanUncrouch() const
{
	return Setup.bCrouchingEnabled;
	// const float Clearance = GetClearanceAbovePawn();
	// return Clearance > Setup.UncrouchClearance;
}

bool ABHCharacter::IsCrouched() const
{
	return bIsCrouched;
}

void ABHCharacter::Sprint()
{
	if (!CanSprint())
	{
		return;
	}
	GetCharacterMovementComponent()->bWantsToSprint = true;
}

void ABHCharacter::EndSprint()
{
	if (!CanEndSprint())
	{
		return;
	}
	GetCharacterMovementComponent()->bWantsToSprint = false;
}

bool ABHCharacter::CanSprint() const
{
	return Setup.bSprintingEnabled;
}

bool ABHCharacter::CanEndSprint() const
{
	return Setup.bSprintingEnabled;
}

bool ABHCharacter::IsSprinting() const
{
	return bSprinting;
}

void ABHCharacter::Die()
{
	if (!CanDie())
	{
		return;
	}
	OnDeath.Broadcast();
}

bool ABHCharacter::CanDie() const
{
	return true;
}

bool ABHCharacter::IsDead() const
{
	return bDead;
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

UBHCharacterMovementComponent* ABHCharacter::GetCharacterMovementComponent() const
{
	return CastChecked<UBHCharacterMovementComponent>(GetCharacterMovement());
}

const FBHCharacterSetup& ABHCharacter::GetSetup() const
{
	return Setup;
}

FBHCharacterSetup& ABHCharacter::GetSetup()
{
	return Setup;
}

