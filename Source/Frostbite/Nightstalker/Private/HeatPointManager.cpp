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
		World->GetTimerManager().SetTimer(HeatPointProcessorTimerHandle, this, &UHeatPointManager::UpdateHeatPointLifeTime, 1.0f, true);
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

inline float CombineHeatValues(float HeatValueA, float HeatValueB)
{
	const float CombinedHeatValue {FMath::Loge(FMath::Exp(HeatValueA) + FMath::Exp(HeatValueB))};
	return CombinedHeatValue;
}

void UHeatPointManager::UpdateHeatPoints(TArray<FHeatPointOverlapData>& OverlapData)
{
	TArray<FHeatPointOverlapData> CombinedOverlapData;
	
	for (const FHeatPointOverlapData& Overlap : OverlapData)
	{
		bool FoundExistingHeatPoint {false};
		
		for (FHeatPointOverlapData& CombinedOverlap : CombinedOverlapData)
		{
			if (CombinedOverlap.HeatPoint == Overlap.HeatPoint)
			{
				
				CombinedOverlap.Event.Heat = CombineHeatValues(CombinedOverlap.Event.Heat, Overlap.Event.Heat);
				FoundExistingHeatPoint = true;
				break;
			}
		}
		
		if (!FoundExistingHeatPoint)
		{
			CombinedOverlapData.Add(Overlap);
		}
	}

	for (AHeatPoint* HeatPoint : HeatPoints)
	{
		for (const FHeatPointOverlapData& CombinedOverlap : CombinedOverlapData)
		{
			if (CombinedOverlap.HeatPoint == HeatPoint)
			{
				HeatPoint->AddHeat(CombinedOverlap.Event.Heat);
				break;
			}
		}
	}
}

void UHeatPointManager::UpdateHeatPointLifeTime()
{
	if (HeatPoints.IsEmpty()) { return; }

	/** Reverse for loop to be able to safely remove elements at an index. */
	for (int32 Index {HeatPoints.Num() - 1}; Index >= 0; --Index)
	{
		if (AHeatPoint* HeatPoint {HeatPoints[Index]}; IsValid(HeatPoint))
		{
			HeatPoint->UpdateLifeTime(1);
			if (HeatPoint->GetExpirationTime() == 0)
			{
				HeatPoints.RemoveAt(Index);
				HeatPoint->Destroy();
			}
		}
	}
}
