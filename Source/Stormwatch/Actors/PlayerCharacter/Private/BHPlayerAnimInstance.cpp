// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHPlayerAnimInstance.h"

#include "BHPlayerCharacter.h"
#include "BHPlayerController.h"
#include "BHPlayerMovementComponent.h"

#include "KismetAnimationLibrary.h"

void UBHPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	const ABHPlayerCharacter* PlayerCharacter = Cast<ABHPlayerCharacter>(GetSkelMeshComponent()->GetOwner());
	check(PlayerCharacter);
	
	const ABHPlayerCharacterController* Controller = Cast<ABHPlayerCharacterController>(PlayerCharacter->GetController());
	const UBHPlayerMovementComponent* CharacterMovement = PlayerCharacter->GetPlayerMovementComponent();
	if (Controller && CharacterMovement)
	{
		CheckMovementState(*PlayerCharacter, *Controller, *CharacterMovement);
	}

	Direction = GetDirection(*PlayerCharacter);
	Speed = GetSpeed(*PlayerCharacter, *CharacterMovement);

	if (bIsFalling ^ CharacterMovement->bIsFalling() && !bIsFalling)
	{
		FallTime = 0.0f;
	}

	bIsFalling = CharacterMovement->bIsFalling();
	bIsAirborne = CharacterMovement->bIsFalling() || CharacterMovement->GetIsJumping();

	if (bIsFalling)
	{
		UpdateFallTime(DeltaSeconds);
	}
		
	CheckTurnInplaceConditions(*PlayerCharacter);
	
	Super::NativeUpdateAnimation(DeltaSeconds);
}

void UBHPlayerAnimInstance::GetStepData(FBHStepData& StepData, const FVector Location)
{
	StepData.Location = Location;

	if (GetSkelMeshComponent() && GetSkelMeshComponent()->GetOwner())
	{
		StepData.Velocity = GetSkelMeshComponent()->GetOwner()->GetVelocity().Length();
	}
		
	FHitResult HitResult;
	FVector TraceStart = Location;
	FVector TraceEnd = Location - FVector(0, 0, 50);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwningActor());
	Params.bTraceComplex = false;
	Params.bReturnPhysicalMaterial = true;

	if (GetWorld() && GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, Params))
	{
		StepData.Object = HitResult.GetActor();
		StepData.PhysicalMaterial = HitResult.PhysMaterial.Get();
	}
}

FBHStepData UBHPlayerAnimInstance::GetFootstepData(const EBHLeftRight Foot)
{
	FBHStepData StepData = FBHStepData();
	const FName Socket = Foot == EBHLeftRight::Left ? "foot_l_socket" : "foot_r_socket";
	if (GetSkelMeshComponent() && GetSkelMeshComponent()->GetOwner())
	{
		FVector Location = GetSkelMeshComponent()->GetSocketLocation(Socket);
		GetStepData(StepData, Location);
	}
	return StepData;
}

FBHStepData UBHPlayerAnimInstance::GetHandstepData(const EBHLeftRight Hand)
{
	FBHStepData StepData = FBHStepData();
	const FName Socket = Hand == EBHLeftRight::Left ? "hand_l_socket" : "hand_r_socket";
	if (GetSkelMeshComponent() && GetSkelMeshComponent()->GetOwner())
	{
		FVector Location = GetSkelMeshComponent()->GetSocketLocation(Socket);
		GetStepData(StepData, Location);
	}
	return StepData;
}

void UBHPlayerAnimInstance::CheckMovementState(const ABHPlayerCharacter& Character,
	const ABHPlayerCharacterController& Controller,const UBHPlayerMovementComponent& CharacterMovement)
{
	bIsMovementPending = Controller.GetHasMovementInput();
	bIsMoving = bIsMovementPending && (CharacterMovement.IsMovingOnGround() || CharacterMovement.bIsFalling());
	bIsCrouching = CharacterMovement.bIsCrouching();

	bDoSprintSop = !bIsMovementPending && Speed > 275 && (Direction >= -20 && Direction <= 20);
}

void UBHPlayerAnimInstance::CheckTurnInplaceConditions(const ABHPlayerCharacter& Character)
{
	if (Character.GetIsTurningInPlace())
	{
		if (Character.GetYawDelta() > 0)
		{
			bIsTurningRight = true;
			bIsTurningLeft = false;
		}
		else
		{
			bIsTurningRight = false;
			bIsTurningLeft = true;
		}
		TurnSpeed = FMath::Clamp(0.1f * abs(Character.GetYawDelta()), 0.0f, 1.0f);
	}
	else
	{
		bIsTurningLeft = false;
		bIsTurningRight = false;
		TurnSpeed = 0.0f;
	}
}

float UBHPlayerAnimInstance::GetDirection(const ABHPlayerCharacter& Character)
{
	const float UnmappedDirection = UKismetAnimationLibrary::CalculateDirection(
		Character.GetVelocity(), Character.GetActorRotation());

	return FMath::GetMappedRangeValueClamped(
		FVector2D(-171.5, 171.5), FVector2D(-180, 180), UnmappedDirection);
}

float UBHPlayerAnimInstance::GetSpeed(const ABHPlayerCharacter& Character, const UBHPlayerMovementComponent& CharacterMovement)
{
	if (!CharacterMovement.GetLastInputVector().IsNearlyZero())
	{
		return Character.GetVelocity().Size2D();
	}
	return 0.0f;
}

void UBHPlayerAnimInstance::UpdateFallTime(const float DeltaTime)
{
	FallTime += DeltaTime;
	VerticalAlpha = FMath::Clamp(FallTime, 0, 1);
}
