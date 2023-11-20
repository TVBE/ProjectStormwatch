// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHPlayerAnimInstance.h"

#include "BHPlayerCharacter.h"
#include "BHPlayerController.h"
#include "BHPlayerMovementComponent.h"

#include "KismetAnimationLibrary.h"

UBHPlayerAnimInstance::UBHPlayerAnimInstance()
{
}

void UBHPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	const ABHPlayerCharacter* PlayerCharacter = Cast<ABHPlayerCharacter>(GetSkelMeshComponent()->GetOwner());
	check(PlayerCharacter);
	
	const ABHPlayerController* Controller = Cast<ABHPlayerController>(PlayerCharacter->GetController());
	const UBHPlayerMovementComponent* CharacterMovement = PlayerCharacter->GetPlayerMovementComponent();
	if (Controller && CharacterMovement)
	{
		CheckMovementState(*Controller, *CharacterMovement);
	}

	Direction = GetDirection(*PlayerCharacter);
	Speed = GetSpeed(*PlayerCharacter, *CharacterMovement);

	if (bIsFalling ^ CharacterMovement->IsFalling() && !bIsFalling)
	{
		FallTime = 0.0f;
	}

	bIsFalling = CharacterMovement->IsFalling();
	bIsAirborne = CharacterMovement->IsFalling() || CharacterMovement->GetIsJumping();

	if (bIsFalling)
	{
		UpdateFallTime(DeltaSeconds);
	}
		
	CheckTurnInplaceConditions(*PlayerCharacter);
	
	Super::NativeUpdateAnimation(DeltaSeconds);
}

void UBHPlayerAnimInstance::GetExtremityData(FBHStepData& StepData, const EBHBodyExtremity BodyPart)
{
	FName Socket;

	switch(BodyPart)
	{
	case EBHBodyExtremity::LeftFoot:
		Socket = "foot_l_socket";
		break;
	case EBHBodyExtremity::RightFoot:
		Socket = "foot_r_socket";
		break;
	case EBHBodyExtremity::LeftHand:
		Socket = "hand_l_socket";
		break;
	case EBHBodyExtremity::RightHand:
		Socket = "hand_r_socket";
		break;
	}

	if (GetSkelMeshComponent() && GetSkelMeshComponent()->GetOwner())
	{
		StepData.Location = GetSkelMeshComponent()->GetSocketLocation(Socket);
		StepData.Velocity = GetSkelMeshComponent()->GetOwner()->GetVelocity().Length();
	}
    
	FHitResult HitResult;
	FVector TraceStart = StepData.Location;
	FVector TraceEnd = StepData.Location - FVector(0, 0, 50);
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

void UBHPlayerAnimInstance::CheckMovementState(const ABHPlayerController& Controller,
	const UBHPlayerMovementComponent& CharacterMovement)
{
	bIsMovementPending = Controller.GetHasMovementInput();
	bIsMoving = bIsMovementPending && (CharacterMovement.IsMovingOnGround() || CharacterMovement.IsFalling());
	bIsCrouching = CharacterMovement.IsFalling();

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