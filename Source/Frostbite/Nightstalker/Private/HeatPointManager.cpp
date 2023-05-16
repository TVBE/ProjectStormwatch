// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#include "HeatPointManager.h"

#include "HeatPoint.h"

DEFINE_LOG_CATEGORY_CLASS(UHeatPointManager, LogHeatPointManager);

UHeatPointManager::UHeatPointManager()
{
}

void UHeatPointManager::Initialize(UNightstalkerDirector* Subsystem)
{
	if (!Subsystem) { return; }

	Director = Subsystem;
	
	if (const UWorld* World {GetWorld()})
	{
		World->GetTimerManager().SetTimer(HeatPointProcessorTimerHandle, this, &UHeatPointManager::ProcessHeatPoints, 1.0f, true);
		UE_LOG(LogHeatPointManager, Log, TEXT("Initialized heat point manager."))
	}
}

void UHeatPointManager::Deinitialize()
{
	if (const UWorld* World {GetWorld()})
	{
		if (World->GetTimerManager().IsTimerActive(HeatPointProcessorTimerHandle))
		{
			World->GetTimerManager().ClearTimer(HeatPointProcessorTimerHandle);
		}
	}
}

void UHeatPointManager::RegisterHeatPoint(AHeatPoint* Instance)
{
	if (!Instance) { return; }
	HeatPoints.AddUnique(Instance);
}

void UHeatPointManager::UnregisterHeatPoint(AHeatPoint* Instance)
{
	if (!Instance) { return; }
	HeatPoints.Remove(Instance);
}

void UHeatPointManager::ProcessHeatPoints()
{
	if (HeatPoints.IsEmpty()) { return; }

	/** Reverse for loop to be able to safely remove elements at an index. */
	for (int32 Index {HeatPoints.Num() - 1}; Index >= 0; --Index)
	{
		if (AHeatPoint* HeatPoint {HeatPoints[Index]}; HeatPoint->DecrementLifeTime(1.0f) == 0)
		{
			HeatPoint->Destroy();
			HeatPoints.RemoveAt(Index);
		}
	}
}
