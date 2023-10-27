// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHPhysicsInteractionComponent.h"

#include "BHInteractionComponent.h"

#include "PhysicsEngine/BodyInstance.h"

UBHPhysicsInteractionComponent::UBHPhysicsInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	bAllowOnlyOneInstancePerActor = true;
}

void UBHPhysicsInteractionComponent::OnRegister()
{
	Super::OnRegister();
	
	bIsGrabbed = true;

	if (const AActor* Actor = GetOwner())
	{
		if (const UBHInteractionComponent* MeshInteractionComponent = Actor->FindComponentByClass<UBHInteractionComponent>())
		{
			Mesh = Cast<UStaticMeshComponent>(MeshInteractionComponent->GetAttachParent());
			if (Mesh)
			if (Mesh->IsSimulatingPhysics())
			{
					Mesh->BodyInstance.bUseCCD = true;
				
					Mesh->SetNotifyRigidBodyCollision(false);
				
					if (!Mesh->BodyInstance.bGenerateWakeEvents)
					{
						Mesh->BodyInstance.bGenerateWakeEvents = true;
						bDisableGenerateWakeEventsOnSleep = true;
					}
				
					OriginalSleepFamily = Mesh->BodyInstance.SleepFamily;
					OriginalSleepThreshold = Mesh->BodyInstance.CustomSleepThresholdMultiplier;
				
					Mesh->BodyInstance.SleepFamily = ESleepFamily::Custom;
					Mesh->BodyInstance.CustomSleepThresholdMultiplier = 1000.0f;
			}
		}
	}
}

void UBHPhysicsInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Mesh)
	{
		Mesh->OnComponentSleep.AddDynamic(this, &UBHPhysicsInteractionComponent::HandleActorSleep);
	}
	
	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(CollisionHitEventEnableTimerHandle, this,
			&UBHPhysicsInteractionComponent::EnableNotifyRigidBodyCollisionOnOwner, CollisionHitEventEnableDelay, false);
	}

	if (Mesh && !Mesh->IsAnyRigidBodyAwake())
	{
		Mesh->WakeAllRigidBodies();
	}
}

void UBHPhysicsInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	/** Temporary implementation to prevent the object from sleeping on release. */
	// if (Mesh && !Mesh->IsAnyRigidBodyAwake())
	// {
	// 	Mesh->WakeAllRigidBodies();
	// }
}

void UBHPhysicsInteractionComponent::HandleOnOwnerGrabbed()
{
	if (!Mesh) { return; }
	
	bIsGrabbed = true;

	if (Mesh->IsSimulatingPhysics())
	{
		Mesh->SetNotifyRigidBodyCollision(false);
		
		if (const UWorld* World = GetWorld())
		{
			if (World->GetTimerManager().IsTimerActive(CollisionHitEventEnableTimerHandle))
			{
				World->GetTimerManager().ClearTimer(CollisionHitEventEnableTimerHandle);
			}

			World->GetTimerManager().SetTimer(CollisionHitEventEnableTimerHandle, this,
				&UBHPhysicsInteractionComponent::EnableNotifyRigidBodyCollisionOnOwner, CollisionHitEventEnableDelay, false);
		}
	}
}

void UBHPhysicsInteractionComponent::HandleOnOwnerReleased()
{
	if (Mesh && !Mesh->IsAnyRigidBodyAwake())
	{
		if (!Mesh->IsAnyRigidBodyAwake())
		{
			Mesh->WakeAllRigidBodies();
		}

		if (const UWorld* World = GetWorld())
		{
			if (World->GetTimerManager().IsTimerActive(RigidBodySleepEnableTimerHandle))
			{
				World->GetTimerManager().ClearTimer(RigidBodySleepEnableTimerHandle);
			}

			World->GetTimerManager().SetTimer(RigidBodySleepEnableTimerHandle, this,
				&UBHPhysicsInteractionComponent::EnableRigidBodySleep, TimeToStayAwakeAfterRelease, false);
		}
		
	}
	
	bIsGrabbed = false;
}

void UBHPhysicsInteractionComponent::EnableNotifyRigidBodyCollisionOnOwner()
{
	if (!Mesh) { return; }
	
	if (Mesh->IsSimulatingPhysics())
	{
		Mesh->SetNotifyRigidBodyCollision(true);
	}
}

void UBHPhysicsInteractionComponent::EnableRigidBodySleep()
{
	if (Mesh)
	{
		Mesh->BodyInstance.SleepFamily = OriginalSleepFamily;
		Mesh->BodyInstance.CustomSleepThresholdMultiplier = OriginalSleepThreshold;
	}
}

void UBHPhysicsInteractionComponent::HandleActorSleep(UPrimitiveComponent* Component, FName BoneName)
{
	if (!Mesh || bIsGrabbed) { return; }
	Mesh->OnComponentSleep.RemoveDynamic(this, &UBHPhysicsInteractionComponent::HandleActorSleep);

	if (bDisableGenerateWakeEventsOnSleep)
	{
		Mesh->BodyInstance.bGenerateWakeEvents = false;
	}
	
	Mesh->BodyInstance.bUseCCD = false;
	
	DestroyComponent();
}




