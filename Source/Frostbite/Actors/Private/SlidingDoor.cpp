// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#include "SlidingDoor.h"
#include "PowerConsumerComponent.h"

ASlidingDoor::ASlidingDoor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

}

void ASlidingDoor::BeginPlay()
{
	Super::BeginPlay();

	if (RequiresPower)
	{
		PowerConsumerComponent = Cast<UPowerConsumerComponent>
		(AddComponentByClass(UPowerConsumerComponent::StaticClass(), false, FTransform(), true));
		if (PowerConsumerComponent)
		{
			PowerConsumerComponent->PowerSource = PowerSource;
			PowerConsumerComponent->OnPowerStateChanged.AddDynamic(this, &ASlidingDoor::EventOnPowerStateChanged);

			PowerConsumerComponent->RegisterComponent();
			PowerConsumerComponent->InitializeComponent();
		}
	}
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

bool ASlidingDoor::Open_Implementation(const AActor* Initiator)
{
	if (RequiresPower && PowerConsumerComponent && PowerConsumerComponent->GetIsPowered())
	{
		EventDoorOpen();
		return true;
	}
	if (!RequiresPower)
	{
		EventDoorOpen();
		return true;
	}
	return false;
}

bool ASlidingDoor::Close_Implementation(const AActor* Initiator)
{
	if (RequiresPower && PowerConsumerComponent && PowerConsumerComponent->GetIsPowered())
	{
		EventDoorClose();
		return true;
	}
	if (!RequiresPower)
	{
		EventDoorClose();
		return true;
	}
	return false;
}

bool ASlidingDoor::Trigger_Implementation(const AActor* Initiator)
{
	Execute_Open(this, this);
	return true;
}

bool ASlidingDoor::Untrigger_Implementation(const AActor* Initiator)
{
	Execute_Close(this, this);
	return true;
}

void ASlidingDoor::EventDoorOpen_Implementation()
{
	DoorState = EDoorState::Opening;
	OnDoorStateChanged.Broadcast(EDoorState::Opening);
}

void ASlidingDoor::EventDoorOpened_Implementation()
{
	DoorState = EDoorState::Open;
	OnDoorStateChanged.Broadcast(EDoorState::Open);
}

void ASlidingDoor::EventDoorClose_Implementation()
{
	DoorState = EDoorState::Closing;
	OnDoorStateChanged.Broadcast(EDoorState::Closing);
}

void ASlidingDoor::EventDoorClosed_Implementation()
{
	DoorState = EDoorState::Closed;
	OnDoorStateChanged.Broadcast(EDoorState::Closed);
}

void ASlidingDoor::EventOnPowerStateChanged_Implementation(const bool NewState)
{
}