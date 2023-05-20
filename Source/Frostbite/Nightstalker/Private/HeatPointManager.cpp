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
				CombinedOverlap.Event.Location = (CombinedOverlap.Event.Location + Overlap.Event.Location) / 2.0f;
				CombinedOverlap.Event.Radius = (CombinedOverlap.Event.Radius + Overlap.Event.Radius) / 2.0f;
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
		for (FHeatPointOverlapData& CombinedOverlap : CombinedOverlapData)
		{
			if (CombinedOverlap.HeatPoint == HeatPoint)
			{
				HeatPoint->AddHeat(CombinedOverlap.Event.Heat);
				HeatPoint->SetActorLocation(CombinedOverlap.Event.Location);
				
				HeatPoint->SetRadius(CombinedOverlap.Event.Radius);
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