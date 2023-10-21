// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHNightstalkerDirector.h"

#include "BHHeatPointManager.h"
#include "BHNightstalker.h"
#include "BHNightstalkerController.h"
#include "SensoryEventManager.h"

DEFINE_LOG_CATEGORY_CLASS(UBHNightstalkerDirector, LogNightstalkerDirector);

void UBHNightstalkerDirector::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	SensoryEventManager = NewObject<USensoryEventManager>(this);
	HeatPointmanager = NewObject<UBHHeatPointManager>(this);

	if (HeatPointmanager)
	{
		HeatPointmanager->Initialize(this);
	}

	UE_LOG(LogNightstalkerDirector, Log, TEXT("Initialized Nightstalker Director."))
}

void UBHNightstalkerDirector::Deinitialize()
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

void UBHNightstalkerDirector::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
}

void UBHNightstalkerDirector::RegisterNightstalker(ANightstalker* Instance)
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

	if (ABHNightstalkerController* NightstalkerController = Cast<ABHNightstalkerController>(Instance->GetController()))
	{
		NightstalkerController->OnPlayerPerceptionChanged.AddDynamic(this, &UBHNightstalkerDirector::HandlePlayerPerceptionChanged);
	}
	
	UE_LOG(LogNightstalkerDirector, Verbose, TEXT("Registered Nightstalker instance: '%s'"), *Instance->GetName());
}

void UBHNightstalkerDirector::UnregisterNightstalker(ANightstalker* Instance)
{
	if (!Instance && Nightstalker != Instance) { return; }
	Nightstalker = nullptr;

	if (SensoryEventManager)
	{
		SensoryEventManager->Nightstalker = nullptr;
	}

	UE_LOG(LogNightstalkerDirector, Verbose, TEXT("Unregistered Nightstalker instance."))
}

void UBHNightstalkerDirector::HandlePlayerPerceptionChanged(bool IsPlayerDetected)
{
	if (HeatPointmanager)
	{
		if (IsPlayerDetected)
		{
			if (HeatPointmanager->IsActive())
			{
				HeatPointmanager->Deactivate();
				HeatPointmanager->FlushHeatPoints();
			}
		}
		else
		{
			if (!HeatPointmanager->IsActive())
			{
				HeatPointmanager->Activate();
			}
		}
	}
}



