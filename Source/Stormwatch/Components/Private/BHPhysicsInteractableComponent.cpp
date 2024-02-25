// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHPhysicsInteractableComponent.h"

#include "BHInteractableComponent.h"

#include "PhysicsEngine/BodyInstance.h"

DEFINE_LOG_CATEGORY_STATIC(LogPhysicsInteractionComponent, Log, All);

UBHPhysicsInteractableComponent::UBHPhysicsInteractableComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
#if WITH_EDITOR
	bAllowOnlyOneInstancePerActor = true;
#endif
}

void UBHPhysicsInteractableComponent::OnRegister()
{
	Super::OnRegister();
#if WITH_EDITOR
	ensureAlwaysMsgf(Cast<UPrimitiveComponent>(GetAttachParent()),
		TEXT("UBHPhysicsInteractableComponent is not attached to a UPrimitiveComponent."));
#endif
}

void UBHPhysicsInteractableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UBHPhysicsInteractableComponent::HandleOnInteractionStart()
{
	bBeingInteractedWith = true;
	
	UPrimitiveComponent* PrimitiveComponent = GetPrimitiveComponent();
	ensure(PrimitiveComponent->IsSimulatingPhysics());
	
	Snapshot.Emplace(FBHCollisionSettingsSnapshot(PrimitiveComponent));
	PrimitiveComponent->BodyInstance.bUseCCD = true;
	PrimitiveComponent->SetNotifyRigidBodyCollision(false);
	PrimitiveComponent->BodyInstance.bGenerateWakeEvents = true;
	PrimitiveComponent->BodyInstance.SleepFamily = ESleepFamily::Custom;
	PrimitiveComponent->BodyInstance.CustomSleepThresholdMultiplier = 1000.0f;
	
	PrimitiveComponent->OnComponentSleep.AddDynamic(this, &UBHPhysicsInteractableComponent::HandleRigidBodySleep);
	
	if (const UWorld* World = GetWorld(); ensure(World))
	{
		PrimitiveComponent->SetNotifyRigidBodyCollision(false);
		
		if (World->GetTimerManager().IsTimerActive(CollisionHitEventEnableTimerHandle))
		{
			World->GetTimerManager().ClearTimer(CollisionHitEventEnableTimerHandle);
		}
		World->GetTimerManager().SetTimer(CollisionHitEventEnableTimerHandle, this,
			&UBHPhysicsInteractableComponent::EnableNotifyRigidBodyCollisionOnAttachParent,
			TimeToDisableCollisionNotifiesOnInteraction, false);
	}

	if (!PrimitiveComponent->IsAnyRigidBodyAwake())
	{
		PrimitiveComponent->WakeAllRigidBodies();
	}
}

void UBHPhysicsInteractableComponent::HandleOnInteractionEnd()
{
	bBeingInteractedWith = false;
	
	UPrimitiveComponent* PrimitiveComponent = GetPrimitiveComponent();
	ensure(PrimitiveComponent->IsSimulatingPhysics());
	if (PrimitiveComponent && !PrimitiveComponent->IsAnyRigidBodyAwake())
	{
		if (!PrimitiveComponent->IsAnyRigidBodyAwake())
		{
			PrimitiveComponent->WakeAllRigidBodies();
		}

		if (const UWorld* World = GetWorld())
		{
			if (World->GetTimerManager().IsTimerActive(RigidBodySleepEnableTimerHandle))
			{
				World->GetTimerManager().ClearTimer(RigidBodySleepEnableTimerHandle);
			}

			World->GetTimerManager().SetTimer(RigidBodySleepEnableTimerHandle, this,
				&UBHPhysicsInteractableComponent::EnableRigidBodySleep, TimeToStayAwakeAfterRelease, false);
		}
	}
	
	bBeingInteractedWith = false;
}

void UBHPhysicsInteractableComponent::EnableNotifyRigidBodyCollisionOnAttachParent()
{
	UPrimitiveComponent* PrimitiveComponent = GetPrimitiveComponent();
	if (PrimitiveComponent->IsSimulatingPhysics())
	{
		PrimitiveComponent->SetNotifyRigidBodyCollision(true);
	}
}

void UBHPhysicsInteractableComponent::EnableRigidBodySleep()
{
	if (ensure(Snapshot.IsSet()))
	{
		UPrimitiveComponent* PrimitiveComponent = GetPrimitiveComponent();
		PrimitiveComponent->BodyInstance.SleepFamily = Snapshot.GetValue().SleepFamily;
		PrimitiveComponent->BodyInstance.CustomSleepThresholdMultiplier = Snapshot.GetValue().SleepThresholdMultiplier;
	}
}

UPrimitiveComponent* UBHPhysicsInteractableComponent::GetPrimitiveComponent() const
{
	UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(GetAttachParent());
	checkf(PrimitiveComponent, TEXT("%s is attached to %s, but expects to be attached to a UPrimitiveComponent in actor: %s."),
	*GetNameSafe(this),
	*GetNameSafe(GetAttachParent()),
	*GetNameSafe(GetOwner()));

	return PrimitiveComponent;
}

void UBHPhysicsInteractableComponent::HandleRigidBodySleep(UPrimitiveComponent* Component, FName BoneName)
{
	UPrimitiveComponent* PrimitiveComponent = GetPrimitiveComponent();
	
	if (bBeingInteractedWith)
	{
		PrimitiveComponent->WakeAllRigidBodies();
		return;
	}
	
	PrimitiveComponent->OnComponentSleep.RemoveDynamic(this, &UBHPhysicsInteractableComponent::HandleRigidBodySleep);

	if (ensure(Snapshot.IsSet()))
	{
		PrimitiveComponent->BodyInstance.bGenerateWakeEvents = Snapshot.GetValue().bGenerateWakeEvents;
		if (bEnableCDOOnInteraction)
		{
			PrimitiveComponent->BodyInstance.bUseCCD = Snapshot.GetValue().bEnableCDO;
		}
	}
}

#if WITH_EDITOR
void UBHPhysicsInteractableComponent::CheckForErrors()
{
	Super::CheckForErrors();

	if (const UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(GetAttachParent()))
	{
		if (!PrimitiveComponent->IsSimulatingPhysics())
		{
			UE_LOG(LogPhysicsInteractionComponent, Warning,
				TEXT("%s is attached to a UPrimitiveComponent that is not simulating physics in actor: %s"),
				*GetName(), *GetOwner()->GetName())
		}
	}
	else
	{
		UE_LOG(LogPhysicsInteractionComponent, Error, TEXT("%s is not attached to a UPrimitiveComponent in actor: %s."),
			*GetName(), *GetOwner()->GetName())
	}
}
#endif




