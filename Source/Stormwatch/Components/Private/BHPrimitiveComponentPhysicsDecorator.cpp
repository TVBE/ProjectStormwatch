// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHPrimitiveComponentPhysicsDecorator.h"

#include "BHInteractionComponent.h"

#include "PhysicsEngine/BodyInstance.h"

UBHPrimitiveComponentPhysicsDecorator::UBHPrimitiveComponentPhysicsDecorator()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UBHPrimitiveComponentPhysicsDecorator::OnRegister()
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

void UBHPrimitiveComponentPhysicsDecorator::BeginPlay()
{
	Super::BeginPlay();

	if (Mesh)
	{
		Mesh->OnComponentSleep.AddDynamic(this, &UBHPrimitiveComponentPhysicsDecorator::HandleActorSleep);
	}
	
	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(CollisionHitEventEnableTimerHandle, this,
			&UBHPrimitiveComponentPhysicsDecorator::EnableNotifyRigidBodyCollisionOnOwner, CollisionHitEventEnableDelay, false);
	}

	if (Mesh && !Mesh->IsAnyRigidBodyAwake())
	{
		Mesh->WakeAllRigidBodies();
	}
}

void UBHPrimitiveComponentPhysicsDecorator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	/** Temporary implementation to prevent the object from sleeping on release. */
	// if (Mesh && !Mesh->IsAnyRigidBodyAwake())
	// {
	// 	Mesh->WakeAllRigidBodies();
	// }
}

void UBHPrimitiveComponentPhysicsDecorator::HandleOnOwnerGrabbed()
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
				&UBHPrimitiveComponentPhysicsDecorator::EnableNotifyRigidBodyCollisionOnOwner, CollisionHitEventEnableDelay, false);
		}
	}
}

void UBHPrimitiveComponentPhysicsDecorator::HandleOnOwnerReleased()
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
				&UBHPrimitiveComponentPhysicsDecorator::EnableRigidBodySleep, TimeToStayAwakeAfterRelease, false);
		}
		
	}
	
	bIsGrabbed = false;
}

void UBHPrimitiveComponentPhysicsDecorator::EnableNotifyRigidBodyCollisionOnOwner()
{
	if (!Mesh) { return; }
	
	if (Mesh->IsSimulatingPhysics())
	{
		Mesh->SetNotifyRigidBodyCollision(true);
	}
}

void UBHPrimitiveComponentPhysicsDecorator::EnableRigidBodySleep()
{
	if (Mesh)
	{
		Mesh->BodyInstance.SleepFamily = OriginalSleepFamily;
		Mesh->BodyInstance.CustomSleepThresholdMultiplier = OriginalSleepThreshold;
	}
}

void UBHPrimitiveComponentPhysicsDecorator::HandleActorSleep(UPrimitiveComponent* Component, FName BoneName)
{
	if (!Mesh || bIsGrabbed) { return; }
	Mesh->OnComponentSleep.RemoveDynamic(this, &UBHPrimitiveComponentPhysicsDecorator::HandleActorSleep);

	if (bDisableGenerateWakeEventsOnSleep)
	{
		Mesh->BodyInstance.bGenerateWakeEvents = false;
	}
	
	Mesh->BodyInstance.bUseCCD = false;
	
	DestroyComponent();
}




