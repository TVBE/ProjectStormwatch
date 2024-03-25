// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHCharacterMovementComponent.h"

#include "BHPlayerCharacter.h"
#include "BHPlayerDragComponent.h"
#include "BHPlayerGrabComponent.h"
#include "GameFramework/PlayerController.h"

UBHCharacterMovementComponent::UBHCharacterMovementComponent()
{
	NavAgentProps.bCanCrouch = true;
}

void UBHCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!GetOwner())
	{
		return;
	}
	UpdateWalkSpeed(DeltaTime);
}

void UBHCharacterMovementComponent::UpdateWalkSpeed(float DeltaTime)
{
	WalkSpeedData.Multiplier = GetWalkSpeedMultiplier();
	WalkSpeedData.ScaledSpeed = WalkSpeedData.BaseSpeed * WalkSpeedData.Multiplier;
	MaxWalkSpeed = WalkSpeedData.ScaledSpeed;
	MaxWalkSpeedCrouched = WalkSpeedData.ScaledSpeed;
}

float UBHCharacterMovementComponent::GetWalkSpeedMultiplier() const
{
	const UBHPlayerGrabComponent* GrabComponent = GetPlayerCharacter()->GetGrabComponent();
	const UBHPlayerDragComponent* DragComponent = GetPlayerCharacter()->GetDragComponent();
	
	if (!GrabComponent->GetGrabbedComponent() || !DragComponent->GetGrabbedComponent()) { return; }

	const UPrimitiveComponent* PrimitiveComponent = GrabComponent->GetGrabbedComponent() ? GrabComponent->GetGrabbedComponent() : DragComponent->GetGrabbedComponent();

	const float Mass = PrimitiveComponent->GetMass();
	const FBoxSphereBounds Bounds = PrimitiveComponent->CalcBounds(PrimitiveComponent->GetComponentTransform());
	const float BoundingBoxSize = Bounds.GetBox().GetVolume();

	const float MassRotationMultiplier = FMath::GetMappedRangeValueClamped
		(MovementSetup.InteractionSpeedWeightRange, MovementSetup.InteractionSpeedWeightScalars, Mass);
	const float BoundsRotationMultiplier = (FMath::GetMappedRangeValueClamped
		(MovementSetup.InteractionSpeedSizeRange, MovementSetup.InteractionSpeedSizeScalars, BoundingBoxSize);

	InteractionMultiplier *= FMath::Clamp(MovementSetup.MassRotationMultiplier * MovementSetup.BoundsRotationMultiplier,
										  InteractionSpeedFloor, 1.0);
}

void UBHCharacterMovementComponent::ProcessLanded(const FHitResult& Hit, float remainingTime, int32 Iterations)
{
	/*if (Velocity.Z < -800)
	{
		if (Velocity.Z < -1000)
		{
			OnLanding.Broadcast(EPlayerLandingType::Heavy);
		}
		else
		{
			OnLanding.Broadcast(EPlayerLandingType::Hard);
		}
	}
	else
	{
		OnLanding.Broadcast(EPlayerLandingType::Soft);
	}*/

	OnLanding.Broadcast(EBHPlayerLandingType::Soft);
	
	Super::ProcessLanded(Hit, remainingTime, Iterations);
}

ABHPlayerCharacter* UBHCharacterMovementComponent::GetPlayerCharacter() const
{
	return Cast<ABHPlayerCharacter>(GetCharacterOwner());
}

void UBHCharacterMovementComponent::Jump()
{
	if (CanJump)
	{
		if (DoJump(false))
		{
			OnJump.Broadcast();
		}
	}
}

bool UBHCharacterMovementComponent::CanJump() const
{
	if (MovementSetup.bJumpingEnabled)
	{
		if (MovementSetup.bRequireJumpClearance && !GetPlayerCharacter()->GetClearanceAbovePawn() > MovementSetup.JumpClearance)
		{
			return false;
		}
	}
	return false;
	

	return ((Clearance > RequiredClearance || Clearance == -1.f) && bJumpingEnabled && !GetMovementComponent()->bIsFalling());
}

void UBHCharacterMovementComponent::StartSprinting()
{
	WalksSpeedData.TargetBaseSpeed = MovementSetup.SprintSpeed;
}

void UBHCharacterMovementComponent::StopSprinting()
{
	WalksSpeedData.TargetBaseSpeed = MovementSetup.WalkSpeed;
}

bool UBHCharacterMovementComponent::IsSprinting() const
{
	return bSprinting && IsMovingOnWalks();
}

bool UBHCharacterMovementComponent::CanSprint() const
{
	return false;
}

void UBHCharacterMovementComponent::StartCrouching()
{
	WalksSpeedData.TargetBaseSpeed = MovementSetup.CrouchSpeed;
	GetPlayerCharacter()->Crouch(false);
	bCrouching = true;
	
}

void UBHCharacterMovementComponent::StopCrouching()
{
	WalksSpeedData.TargetBaseSpeed = MovementSetup.WalkSpeed;
	GetPlayerCharacter()->UnCrouch(false);
	bCrouching = false;
}

bool UBHCharacterMovementComponent::IsCrouching() const
{
	return bCrouching;
}

bool UBHCharacterMovementComponent::CanCrouch() const
{
	return !IsCrouching() && !IsFalling() && !IsJumping();
}

bool UBHCharacterMovementComponent::CanUncrouch() const
{
	if (IsCrouching())
	{
		const float Clearance = GetPlayerCharacter()->GetClearanceAbovePawn();
		return Clearance > MovementSetup.UncrouchClearance;
	}
	return false;
}




