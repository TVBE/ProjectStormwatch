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

	Activate();
	UE_LOG(LogHeatPointManager, Log, TEXT("Initialized heat point manager."))
}

void UHeatPointManager::Deinitialize()
{
	Deactivate();
	FlushHeatPoints();
}

void UHeatPointManager::Activate()
{
	if (const UWorld* World {GetWorld()})
	{
		World->GetTimerManager().SetTimer(HeatPointProcessorTimerHandle, this, &UHeatPointManager::UpdateHeatPointLifeTime, 1.0f, true);
		UE_LOG(LogHeatPointManager, Verbose, TEXT("Activated heat point manager."))
	}
}

void UHeatPointManager::Deactivate()
{
	if (const UWorld* World {GetWorld()})
	{
		if (World->GetTimerManager().IsTimerActive(HeatPointProcessorTimerHandle))
		{
			World->GetTimerManager().ClearTimer(HeatPointProcessorTimerHandle);
			UE_LOG(LogHeatPointManager, Verbose, TEXT("Deactivated heat point manager."))
		}
	}
}

void UHeatPointManager::RegisterHeatPoint(AHeatPoint* Instance)
{
	if (!Instance) { return; }
	
	HeatPoints.AddUnique(Instance);

	SortHeatPointsByHeatValue();

	if (HeatPoints.Num() == 1)
	{
		OnHottestHeatPointChanged.Broadcast(Instance);
	}
}

void UHeatPointManager::UnregisterHeatPoint(AHeatPoint* Instance)
{
	if (!Instance) { return; }
	HeatPoints.Remove(Instance);
	SortHeatPointsByHeatValue();
}

inline float CombineHeatValues(float HeatValueA, float HeatValueB)
{
	const float CombinedHeatValue {FMath::Loge(FMath::Exp(HeatValueA) + FMath::Exp(HeatValueB))};
	return CombinedHeatValue;
}

void UHeatPointManager::UpdateHeatPoints(TArray<FHeatPointOverlapData>& OverlapData)
{
	TArray<FHeatPointOverlapData> CombinedOverlapData;
	
	/** Set to track which heat points had their heat increased. */
	TSet<AHeatPoint*> HeatPointsIncreased;

	/** Variable to track total added heat. */
	float TotalAddedHeat {0}; 

	for (const FHeatPointOverlapData& Overlap : OverlapData)
	{
		bool FoundExistingHeatPoint {false};
		
		for (FHeatPointOverlapData& CombinedOverlap : CombinedOverlapData)
		{
			if (CombinedOverlap.HeatPoint == Overlap.HeatPoint)
			{
				const float OldHeat {CombinedOverlap.Event.Heat};
				CombinedOverlap.Event.Heat = CombineHeatValues(CombinedOverlap.Event.Heat, Overlap.Event.Heat);
				TotalAddedHeat += CombinedOverlap.Event.Heat - OldHeat; 
				
				HeatPointsIncreased.Add(CombinedOverlap.HeatPoint);
				
				CombinedOverlap.Event.Location = (CombinedOverlap.Event.Location + Overlap.Event.Location) / 2.0f;
				CombinedOverlap.Event.Radius = (CombinedOverlap.Event.Radius + Overlap.Event.Radius) / 2.0f;
				FoundExistingHeatPoint = true;
				break;
			}
		}
		
		if (!FoundExistingHeatPoint)
		{
			CombinedOverlapData.Add(Overlap);
			TotalAddedHeat += Overlap.Event.Heat;
			
			HeatPointsIncreased.Add(Overlap.HeatPoint);
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
		
		if (!HeatPointsIncreased.Contains(HeatPoint))
		{
			HeatPoint->DetractHeat(TotalAddedHeat * 0.1f);
		}
	}
	
	RemoveZeroHeatPoints();
}


void UHeatPointManager::SortHeatPointsByHeatValue()
{
	const AHeatPoint* PreviousHottest {HeatPoints.Num() > 0 ? HeatPoints[HeatPoints.Num() - 1] : nullptr};

	HeatPoints.Sort([](const AHeatPoint& A, const AHeatPoint& B)
	{
		return A.GetHeat() < B.GetHeat();
	});

	AHeatPoint* CurrentHottest {HeatPoints.Num() > 0 ? HeatPoints[HeatPoints.Num() - 1] : nullptr};

	if (CurrentHottest != PreviousHottest)
	{
		OnHottestHeatPointChanged.Broadcast(CurrentHottest);
	}
}

void UHeatPointManager::FlushHeatPoints()
{
	for (AHeatPoint* HeatPoint : HeatPoints)
	{
		HeatPoint->Destroy();
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

void UHeatPointManager::RemoveZeroHeatPoints()
{
	if (HeatPoints.IsEmpty()) { return; }

	for (int32 Index {HeatPoints.Num() - 1}; Index >= 0; --Index)
	{
		AHeatPoint* HeatPoint {HeatPoints[Index]};
		if (HeatPoint && HeatPoint->GetHeat() <= 0)
		{
			HeatPoints.RemoveAt(Index);
			HeatPoint->Destroy();
		}
	}
}

bool UHeatPointManager::IsActive() const
{
	if (const UWorld* World {GetWorld()})
	{
		if (World->GetTimerManager().IsTimerActive(HeatPointProcessorTimerHandle))
		{
			return true;
		}
	}
	return false;
}

AHeatPoint* UHeatPointManager::GetHottestHeatPoint()
{
	if (HeatPoints.IsEmpty()) { return nullptr; }
	return HeatPoints[0];
}
