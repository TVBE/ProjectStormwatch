// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#include "NightstalkerDirector.h"

#include "HeatPointManager.h"
#include "Nightstalker.h"
#include "NightstalkerController.h"
#include "SensoryEventManager.h"

DEFINE_LOG_CATEGORY_CLASS(UNightstalkerDirector, LogNightstalkerDirector);

void UNightstalkerDirector::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	SensoryEventManager = NewObject<USensoryEventManager>(this);
	HeatPointmanager = NewObject<UHeatPointManager>(this);

	if (HeatPointmanager)
	{
		HeatPointmanager->Initialize(this);
	}

	UE_LOG(LogNightstalkerDirector, Log, TEXT("Initialized Nightstalker Director."))
}

void UNightstalkerDirector::Deinitialize()
{
	Super::Deinitialize();

	if (SensoryEventManager)
	{
		SensoryEventManager->Deinitialize();
		SensoryEventManager->MarkAsGarbage();
		SensoryEventManager = nullptr;
	}
	if (HeatPointmanager)
	{
		HeatPointmanager->Deinitialize();
		HeatPointmanager->MarkAsGarbage();
		HeatPointmanager = nullptr;
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
		
		/** We initialize the SensoryEventManager here as it serves no purpose without a Nightstalker instance in the world.
		 *	We might want to change this later as it is slightly more difficult to debug the sensory event manager without a nightstalker instance. */
		SensoryEventManager->Initialize(this);
	}

	if (ANightstalkerController* NightstalkerController {Cast<ANightstalkerController>(Instance->GetController())})
	{
		NightstalkerController->OnPlayerPerceptionChanged.AddDynamic(this, &UNightstalkerDirector::HandlePlayerPerceptionChanged);
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

void UNightstalkerDirector::HandlePlayerPerceptionChanged(bool IsPlayerDetected)
{
	if (HeatPointmanager)
	{
		if (IsPlayerDetected)
		{
		
		}
		else
		{
		
		}
	}
}



