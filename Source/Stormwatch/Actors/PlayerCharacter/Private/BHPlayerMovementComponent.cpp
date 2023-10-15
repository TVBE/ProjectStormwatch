// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHPlayerMovementComponent.h"
#include "GameFramework/PlayerController.h"

UBHPlayerMovementComponent::UBHPlayerMovementComponent()
{
	NavAgentProps.bCanCrouch = true;
}

void UBHPlayerMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                      FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


}

void UBHPlayerMovementComponent::UpdateMovementSpeed()
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

void UBHPlayerMovementComponent::BeginPlay()
{
	Super::BeginPlay();
}

bool UBHPlayerMovementComponent::DoJump(bool bReplayingMoves)
{
	OnLocomotionEvent.Broadcast(EBHPlayerLocomotionEvent::Jump);
	OnJump.Broadcast();
	return Super::DoJump(bReplayingMoves);
}

void UBHPlayerMovementComponent::ProcessLanded(const FHitResult& Hit, float remainingTime, int32 Iterations)
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

/** Checks the current movement state and returns a corresponding enumeration value. */
EBHPlayerGroundMovementType UBHPlayerMovementComponent::GetGroundMovementType() const
{
	if (bIsSprinting)
	{
		return EBHPlayerGroundMovementType::Sprinting;
	}
	if (IsMovingOnGround() && Velocity.SquaredLength() >= 25)
	{
		return EBHPlayerGroundMovementType::Walking;
	}
	return EBHPlayerGroundMovementType::Idle;
}

void UBHPlayerMovementComponent::SetIsSprinting(bool bValue)
{
	if (!PawnOwner || bIsSprinting == bValue)
	{
		return;
	}
	
	bIsSprinting = bValue;
	
	if (bValue)
	{
		OnLocomotionEvent.Broadcast(EBHPlayerLocomotionEvent::SprintStart);
	}
	else
	{
		OnLocomotionEvent.Broadcast(EBHPlayerLocomotionEvent::SprintEnd);
	}
}




