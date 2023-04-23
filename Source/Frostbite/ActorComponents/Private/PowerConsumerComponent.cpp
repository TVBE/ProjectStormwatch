// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#include "PowerConsumerComponent.h"
#include "PowerSource.h"

UPowerConsumerComponent::UPowerConsumerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPowerConsumerComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (APowerSource* ResolvedPowerSource = PowerSource.Get())
	{
		ResolvedPowerSource->RegisterPowerConsumer(this);
	}
}

bool UPowerConsumerComponent::SetPowerState_Implementation(const bool NewPowerState, const AActor* Initiator)
{
	if (IsPowered != NewPowerState)
	{
		IsPowered = NewPowerState;
		OnPowerStateChanged.Broadcast(NewPowerState);
		return true;
	}
	return false;
}





 