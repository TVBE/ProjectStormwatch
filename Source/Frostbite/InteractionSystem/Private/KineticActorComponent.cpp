// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#include "KineticActorComponent.h"
#include "PhysicsEngine/BodyInstance.h"

UKineticActorComponent::UKineticActorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UKineticActorComponent::OnRegister()
{
	Super::OnRegister();

	if (const AActor* Actor {GetOwner()})
	{
		Mesh = Actor->FindComponentByClass<UStaticMeshComponent>();
		if (Mesh)
		{
			if (Mesh->IsSimulatingPhysics())
			{
					Mesh->BodyInstance.bUseCCD = true;
				
					Mesh->SetNotifyRigidBodyCollision(false);
				
					if (!Mesh->BodyInstance.bGenerateWakeEvents)
					{
						Mesh->BodyInstance.bGenerateWakeEvents = true;
						DisableGenerateWakeEventsOnSleep = true;
					}
				
					Mesh->OnComponentSleep.AddDynamic(this, &UKineticActorComponent::HandleActorSleep);
			}
		}
	}
}

void UKineticActorComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (const UWorld* World {GetWorld()})
	{
		World->GetTimerManager().SetTimer(CollisionHitEventEnableTimerHandle, this,
			&UKineticActorComponent::EnableNotifyRigidBodyCollisionOnOwner, CollisionHitEventEnableDelay, false);
	}
}

void UKineticActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UKineticActorComponent::HandleOnOwnerGrabbed()
{
	if (!Mesh) { return; }

	if (Mesh->IsSimulatingPhysics())
	{
		Mesh->SetNotifyRigidBodyCollision(false);
		
		if (const UWorld* World {GetWorld()})
		{
			if (World->GetTimerManager().IsTimerActive(CollisionHitEventEnableTimerHandle))
			{
				World->GetTimerManager().ClearTimer(CollisionHitEventEnableTimerHandle);
			}

			World->GetTimerManager().SetTimer(CollisionHitEventEnableTimerHandle, this,
				&UKineticActorComponent::EnableNotifyRigidBodyCollisionOnOwner, CollisionHitEventEnableDelay, false);
		}
	}
}

void UKineticActorComponent::EnableNotifyRigidBodyCollisionOnOwner()
{
	if (!Mesh) { return; }
	
	if (Mesh->IsSimulatingPhysics())
	{
		Mesh->SetNotifyRigidBodyCollision(true);
	}
}


void UKineticActorComponent::HandleActorSleep(UPrimitiveComponent* Component, FName BoneName)
{
	if (!Mesh) { return; }
	Mesh->OnComponentSleep.RemoveDynamic(this, &UKineticActorComponent::HandleActorSleep);

	if (DisableGenerateWakeEventsOnSleep)
	{
		Mesh->BodyInstance.bGenerateWakeEvents = false;
	}
	
	Mesh->BodyInstance.bUseCCD = false;
	
	DestroyComponent();
}




