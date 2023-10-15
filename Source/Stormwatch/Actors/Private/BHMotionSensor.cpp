// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHMotionSensor.h"
#include "Components/PrimitiveComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Pawn.h"

ABHMotionSensor::ABHMotionSensor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABHMotionSensor::PostInitProperties()
{
	Super::PostInitProperties();
	DetectionArea->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Overlap);
}

void ABHMotionSensor::Poll()
{
	IsActorDetected = false;

	if (IsBroken) { return; }
	
	if (!OverlappingActors.IsEmpty())
	{
		for (const AActor* Actor : OverlappingActors)
		{
			if (!IsActorOccluded(Actor) && IsActorMoving(Actor))
			{
				IsActorDetected = true;
				break;
			}
		}
	}
	
	if (IsActorDetected)
	{
		
		if (IsTriggered)
		{
			return;
		}

		SetState(EBHSensorState::Detecting);

		if (const UWorld* World = GetWorld())
		{
			if (World->GetTimerManager().IsTimerActive(CooldownTimerHandle))
			{
				World->GetTimerManager().ClearTimer(CooldownTimerHandle);
			}
		}
		
		DetectionLevel += PollInterval;
		if (!IsAlerted)
		{
			IsAlerted = true;
		}
		
		if (DetectionLevel >= DetectionThreshold)
		{
			IsTriggered = true;
			
			if (IsManualResetRequired)
			{
				if (const UWorld* World = GetWorld())
				{
					if (World->GetTimerManager().IsTimerActive(PollTimerHandle))
					{
						World->GetTimerManager().ClearTimer(PollTimerHandle);
					}
				}
			}

			SetState(EBHSensorState::Triggered);
		}
	}
	else
	{
		if (IsAlerted)
		{
			SetState(EBHSensorState::Alerted);
			
			if (const UWorld* World = GetWorld())
			{
				if (World->GetTimerManager().IsTimerActive(CooldownTimerHandle))
				{
					return;
				}
				StartCooldown();
			}
		}
	}
}

bool ABHMotionSensor::IsActorMoving(const AActor* Actor) const
{
	if (UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(Actor->GetRootComponent()))
	{
		const FVector ActorVelocity = PrimitiveComponent->GetPhysicsLinearVelocity();
		return ActorVelocity.Size() >= VelocityThreshold;
	}
	return false;
}


