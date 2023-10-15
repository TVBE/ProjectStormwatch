// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHPowerConsumptionComponent.h"

#include "BHPowerSource.h"

UBHPowerConsumptionComponent::UBHPowerConsumptionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBHPowerConsumptionComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (ABHPowerSource* ResolvedPowerSource = PowerSource.Get())
	{
		ResolvedPowerSource->RegisterPowerConsumer(this);
	}
}

bool UBHPowerConsumptionComponent::SetPowerState_Implementation(const bool NewPowerState, const AActor* Initiator)
{
	if (bPowered != NewPowerState)
	{
		bPowered = NewPowerState;
		OnPowerStateChanged.Broadcast(NewPowerState);
		return true;
	}
	return false;
}





 
