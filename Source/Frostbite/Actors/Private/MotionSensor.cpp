// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#include "MotionSensor.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Pawn.h"

AMotionSensor::AMotionSensor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AMotionSensor::PostInitProperties()
{
	Super::PostInitProperties();
	DetectionArea->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Overlap);
}

void AMotionSensor::Poll()
{
	IsActorDetected = false;
	
	if (OverlappingActors.IsEmpty())
	{
		EventOnPoll();
		return;
	}
	
	for (const AActor* Actor : OverlappingActors)
	{
		if (!IsActorOccluded(Actor) && IsActorMoving(Actor))
		{
			IsActorDetected = true;
			break;
		}
	}
	
	if (IsActorDetected)
	{
		if (IsTriggered)
		{
			EventOnPoll();
			return;
		}

		if (const UWorld* World {GetWorld()})
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
				if (const UWorld* World {GetWorld()})
				{
					if (World->GetTimerManager().IsTimerActive(PollTimerHandle))
					{
						World->GetTimerManager().ClearTimer(PollTimerHandle);
					}
				}
			}
		}
	}
	else
	{
		if (IsAlerted)
		{
			if (const UWorld* World {GetWorld()})
			{
				if (World->GetTimerManager().IsTimerActive(CooldownTimerHandle))
				{
					EventOnPoll();
					return;
				}
				StartCooldown();
			}
		}
	}
	EventOnPoll();
}

bool AMotionSensor::IsActorMoving(const AActor* Actor) const
{
	if (UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(Actor->GetRootComponent()))
	{
		const FVector ActorVelocity = PrimitiveComponent->GetPhysicsLinearVelocity();
		return ActorVelocity.Size() >= VelocityThreshold;
	}
	return false;
}


