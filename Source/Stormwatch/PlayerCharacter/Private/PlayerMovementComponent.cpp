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

void UPlayerMovementComponent::UpdateMovementSpeed()
{
	float InteractionMultiplier = 1.0f;

	if (!GrabComponent->GetGrabbedComponent() || !DragComponent->GetGrabbedComponent()) { return; }

	const UPrimitiveComponent* PrimitiveComponent = GrabComponent->GetGrabbedComponent() ? GrabComponent->GetGrabbedComponent() : DragComponent->GetGrabbedComponent();

	const float Mass = PrimitiveComponent->GetMass();
	const FBoxSphereBounds Bounds = PrimitiveComponent->CalcBounds(PrimitiveComponent->GetComponentTransform());
	const float BoundingBoxSize = static_cast<float>(Bounds.GetBox().GetVolume());

	const float MassRotationMultiplier {static_cast<float>(FMath::GetMappedRangeValueClamped
		(Settings.InteractionSpeedWeightRange, Settings.InteractionSpeedWeightScalars, Mass))};
	const float BoundsRotationMultiplier {static_cast<float>(FMath::GetMappedRangeValueClamped
		(Settings.InteractionSpeedSizeRange, Settings.InteractionSpeedSizeScalars, BoundingBoxSize))};

	InteractionMultiplier *= FMath::Clamp(Settings.MassRotationMultiplier * Settings.BoundsRotationMultiplier,
										  InteractionSpeedFloor, 1.0);

	PlayerMovement->MaxWalkSpeed = ScaledSpeed;
	PlayerMovement->MaxWalkSpeedCrouched = CrouchSpeed * InteractionMultiplier; // TODO: Needs different implementation in future.
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

void UPlayerMovementComponent::SetIsSprinting(bool bValue)
{
	if (!PawnOwner || IsSprinting == bValue)
	{
		return;
	}
	
	IsSprinting = bValue;
	
	if (bValue)
	{
		OnLocomotionEvent.Broadcast(EPlayerLocomotionEvent::SprintStart);
	}
	else
	{
		OnLocomotionEvent.Broadcast(EPlayerLocomotionEvent::SprintEnd);
	}
}




