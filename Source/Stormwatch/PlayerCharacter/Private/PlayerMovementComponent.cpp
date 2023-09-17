// Copyright (c) 2022-present Barrelhouse. All rights reserved.
// Written by Tim Verberne.

#include "PlayerMovementComponent.h"
#include "GameFramework/PlayerController.h"

UPlayerMovementComponent::UPlayerMovementComponent()
{
	NavAgentProps.bCanCrouch = true;
}

void UPlayerMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                      FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPlayerMovementComponent::BeginPlay()
{
	Super::BeginPlay();
}

bool UPlayerMovementComponent::DoJump(bool bReplayingMoves)
{
	OnLocomotionEvent.Broadcast(EPlayerLocomotionEvent::Jump);
	OnJump.Broadcast();
	return Super::DoJump(bReplayingMoves);
}

void UPlayerMovementComponent::ProcessLanded(const FHitResult& Hit, float remainingTime, int32 Iterations)
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

	OnLanding.Broadcast(EPlayerLandingType::Soft);
	
	Super::ProcessLanded(Hit, remainingTime, Iterations);
}

/** Checks the current movement state and returns a corresponding enumeration value. */
EPlayerGroundMovementType UPlayerMovementComponent::GetGroundMovementType() const
{
	if (IsSprinting)
	{
		return EPlayerGroundMovementType::Sprinting;
	}
	if (IsMovingOnGround() && Velocity.SquaredLength() >= 25)
	{
		return EPlayerGroundMovementType::Walking;
	}
	return EPlayerGroundMovementType::Idle;
}

/** Called by the player controller. */
void UPlayerMovementComponent::SetIsSprinting(const bool Value)
{
	if (!PawnOwner || IsSprinting == Value)
	{
		return;
	}
	
	IsSprinting = Value;
	
	if (Value)
	{
		OnLocomotionEvent.Broadcast(EPlayerLocomotionEvent::SprintStart);
	}
	else
	{
		OnLocomotionEvent.Broadcast(EPlayerLocomotionEvent::SprintEnd);
	}
}




