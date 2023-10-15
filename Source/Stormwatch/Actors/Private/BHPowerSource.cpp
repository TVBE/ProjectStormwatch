// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHPowerSource.h"

#include "PowerConsumerInterface.h"

DEFINE_LOG_CATEGORY_CLASS(ABHPowerSource, LogPowerSource)

ABHPowerSource::ABHPowerSource()
{
 	
	PrimaryActorTick.bCanEverTick = false;
}

void ABHPowerSource::BeginPlay()
{
	Super::BeginPlay();
}

void ABHPowerSource::RegisterPowerConsumer(UObject* Consumer)
{
	if (!Consumer)
	{
		UE_LOG(LogPowerSource, Warning, TEXT("RegisterPowerConsumer: Consumer is null"));
		return;
	}

	if (!Consumer->GetClass()->ImplementsInterface(UPowerConsumer::StaticClass()))
	{
		UE_LOG(LogPowerSource, Warning, TEXT("RegisterPowerConsumer: Consumer does not implement IPowerConsumer interface"));
		return;
	}

	if (ConnectedConsumers.Contains(Consumer))
	{
		UE_LOG(LogPowerSource, Verbose, TEXT("RegisterPowerConsumer: Consumer already exists in the array"));
		return;
	}
	
	ConnectedConsumers.Add(Consumer);

	IPowerConsumer::Execute_SetPowerState(Consumer, IsEnergized, this);
}

bool ABHPowerSource::Trigger_Implementation(const AActor* Initiator)
{
	if (!IsEnergized)
	{
		SetPowerSourceState(true);
		UE_LOG(LogPowerSource, Verbose, TEXT("Power has been enabled for %s"), *GetNameSafe(this));
		return true;
	}
	return false;
}

bool ABHPowerSource::Untrigger_Implementation(const AActor* Initiator)
{
	if (IsEnergized)
	{
		SetPowerSourceState(false);
		UE_LOG(LogPowerSource, Verbose, TEXT("Power has been disabled for %s"), *GetNameSafe(this));
		return true;
	}
	return false;
}

void ABHPowerSource::SetPowerSourceState(const bool State)
{
	if (State != IsEnergized)
	{
		IsEnergized = State;
		
		int32 SetPowerStateCount = 0;
		
		if (!ConnectedConsumers.IsEmpty())
		{
			for (UObject* Consumer : ConnectedConsumers)
			{
				if (Consumer)
				{
					IPowerConsumer::Execute_SetPowerState(Consumer, IsEnergized, this);
					SetPowerStateCount++;
				}
			}
		}
		UE_LOG(LogPowerSource, Verbose, TEXT("SetPowerSourceState: Power state has been set for %d objects"), SetPowerStateCount);
	}
}











