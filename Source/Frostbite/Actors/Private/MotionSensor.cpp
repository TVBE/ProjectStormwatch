// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#include "MotionSensor.h"
#include "Components/PrimitiveComponent.h"
#include "Components/BoxComponent.h"
#include "PlayerCharacter.h"
#include "Nightstalker.h"
#include "GameFramework/Pawn.h"

AMotionSensor::AMotionSensor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AMotionSensor::PostInitProperties()
{
	Super::PostInitProperties();
	DetectionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Overlap);
}

void AMotionSensor::Poll()
{
	if (!DetectionBox) { return; }
	DetectionBox->GetOverlappingActors(OverlappingActors, AActor::StaticClass());

	for (int32 Index {0}; Index < OverlappingActors.Num(); ++Index)
	{
		AActor* Actor {OverlappingActors[Index]};
		bool ShouldIgnoreActor {false};
		
		if (Actor)
		{
			if (Actor->IsA(APlayerCharacter::StaticClass()))
			{
				if (IgnoreParameters.Contains(EBProximitySensorIgnoreParameter::Player))
				{
					ShouldIgnoreActor = true;
				}
				else if (IgnoreParameters.Contains(EBProximitySensorIgnoreParameter::Crouching))
				{
					if (const APlayerCharacter* PlayerCharacter {Cast<APlayerCharacter>(Actor)}; PlayerCharacter && PlayerCharacter->bIsCrouched)
					{
						ShouldIgnoreActor = true;
					}
				}
			}
			else if (Actor->IsA(ANightstalker::StaticClass()) && IgnoreParameters.Contains(EBProximitySensorIgnoreParameter::Nightstalker))
			{
				ShouldIgnoreActor = true;
			}

			if (ShouldIgnoreActor)
			{
				OverlappingActors[Index] = nullptr;
			}
			else
			{
				const FVector DirectionToActor {(Actor->GetActorLocation() - this->GetActorLocation()).GetSafeNormal()};
				const float DotProduct {static_cast<float>(FVector::DotProduct(DirectionToActor, Root->GetForwardVector()))};

				if (!(DotProduct > FMath::Cos(FMath::DegreesToRadians(ConeAngle))) || IsActorOccluded(Actor) || !IsActorMoving(Actor))
				{
					OverlappingActors[Index] = nullptr;
				}
			}
		}
	}

	if (OverlappingActors.IsEmpty())
	{
		if (IsActorDetected)
		{
			IsActorDetected = false;
			OnActorLost.Broadcast();
		}
		return;
	}
		

	AActor* NearestActor {nullptr};
	float NearestPawnDistance {FLT_MAX};
	
	for (AActor* OverlappingActor : OverlappingActors)
	{
		if (OverlappingActor)
		{
			const float Distance {static_cast<float>(FVector::Dist(GetActorLocation(), OverlappingActor->GetActorLocation()))};
			if (Distance < NearestPawnDistance)
			{
				NearestPawnDistance = Distance;
				NearestActor = OverlappingActor;
			}
		}
	}

	if (NearestActor)
	{
		IsActorDetected = true;
		OnActorDetected.Broadcast(NearestActor, NearestPawnDistance);
	}
	else if (IsActorDetected)
	{
		IsActorDetected = false;
		OnActorLost.Broadcast();
	}
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


