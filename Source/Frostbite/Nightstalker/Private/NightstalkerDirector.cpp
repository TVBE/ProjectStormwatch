// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#include "NightstalkerDirector.h"

#include "Nightstalker.h"
#include "SensoryEventManager.h"

DEFINE_LOG_CATEGORY_CLASS(UNightstalkerDirector, LogNightstalkerDirector);

void UNightstalkerDirector::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	SensoryEventManager = NewObject<USensoryEventManager>(this);

	UE_LOG(LogNightstalkerDirector, Log, TEXT("Initialized Nightstalker Director."))
}

void UNightstalkerDirector::Deinitialize()
{
	Super::Deinitialize();

	if (SensoryEventManager)
	{
		SensoryEventManager->MarkAsGarbage();
		SensoryEventManager = nullptr;
	}
}

void UNightstalkerDirector::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
}

void UNightstalkerDirector::RegisterNightstalker(ANightstalker* Instance)
{
	if (!Instance) { return; }
	Nightstalker = Instance;
	
	if (SensoryEventManager)
	{
		SensoryEventManager->Nightstalker = Instance;

		/** We initialize the SensoryEventManager here as it serves no purpose without a Nightstalker instance in the world. */
		SensoryEventManager->Initialize();
	}
	UE_LOG(LogNightstalkerDirector, Verbose, TEXT("Registered Nightstalker instance: '%s'"), *Instance->GetName());
}

void UNightstalkerDirector::UnregisterNightstalker(ANightstalker* Instance)
{
	if (!Instance && Nightstalker != Instance) { return; }
	Nightstalker = nullptr;

	if (SensoryEventManager)
	{
		SensoryEventManager->Nightstalker = nullptr;
	}

	UE_LOG(LogNightstalkerDirector, Verbose, TEXT("Unregistered Nightstalker instance."))
}

void UNightstalkerDirector::RegisterHeatPoint(AHeatPoint* Instance)
{
	if (!Instance || HeatPoints.Contains(Instance))
	{
		return;
	}

	HeatPoints.Add(Instance);
}

void UNightstalkerDirector::UnregisterHeatPoint(AHeatPoint* Instance)
{
	if (!Instance)
	{
		return;
	}

	HeatPoints.Remove(Instance);
}

