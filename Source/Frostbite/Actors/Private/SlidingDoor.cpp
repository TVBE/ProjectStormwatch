// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite


#include "SlidingDoor.h"

ASlidingDoor::ASlidingDoor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

}

void ASlidingDoor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASlidingDoor::StartCooldown()
{
	IsCooldownActive = true;
	if (const UWorld* World = GetWorld())
	{
		FTimerManager& TimerManager = World->GetTimerManager();
		TimerManager.SetTimer(CooldownTimerHandle, this, &ASlidingDoor::HandleCooldownFinished, CooldownTime, false);
	}
}

void ASlidingDoor::AddToTriggerQueue(const ETriggerableObjectAction Value)
{
	TriggerQueue.Add(Value);
}

ETriggerableObjectAction ASlidingDoor::EvaluateAndClearTriggerQueue()
{
	const ETriggerableObjectAction LastEntry {TriggerQueue.Last()};
	TriggerQueue.Empty();
	return LastEntry;
}

void ASlidingDoor::HandleCooldownFinished()
{
	IsCooldownActive = false;
}

void ASlidingDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASlidingDoor::Open_Implementation()
{
	IDoor::Open_Implementation();
}

void ASlidingDoor::Close_Implementation()
{
	IDoor::Close_Implementation();
}

bool ASlidingDoor::Trigger_Implementation(const AActor* Initiator)
{
	return ITriggerableObject::Trigger_Implementation(Initiator);
}

bool ASlidingDoor::Untrigger_Implementation(const AActor* Initiator)
{
	return ITriggerableObject::Untrigger_Implementation(Initiator);
}

void ASlidingDoor::EventDoorOpen_Implementation()
{
	OnDoorOpen.Broadcast();
}

void ASlidingDoor::EventDoorOpened_Implementation()
{
	OnDoorOpened.Broadcast();
}

void ASlidingDoor::EventDoorClose_Implementation()
{
	OnDoorClose.Broadcast();
}

void ASlidingDoor::EventDoorClosed_Implementation()
{
	OnDoorClosed.Broadcast();
}