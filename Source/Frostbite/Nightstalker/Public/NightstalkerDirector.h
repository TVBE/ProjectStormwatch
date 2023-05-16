// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "HeatPointManager.h"
#include "Nightstalker.h"
#include "Subsystems/WorldSubsystem.h"
#include "NightstalkerDirector.generated.h"

class USensoryEventManager;
class ANightstalker;
class AHeatPoint;

UCLASS(ClassGroup = "Nightstalker")
class UNightstalkerDirector : public UWorldSubsystem
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogNightstalkerDirector, Log, All)

private:
	UPROPERTY(BlueprintGetter = GetSensoryEventManager)
	USensoryEventManager* SensoryEventManager;

	UPROPERTY(BlueprintGetter = GetHeatPointManager)
	UHeatPointManager* HeatPointmanager;

	UPROPERTY()
	ANightstalker* Nightstalker;

public:
	void RegisterNightstalker(ANightstalker* Instance);
	void UnregisterNightstalker(ANightstalker* Instance);
	
	FORCEINLINE void RegisterHeatPoint(AHeatPoint* Instance)
	{
		if (!Instance || !HeatPointmanager) { return; }
		HeatPointmanager->RegisterHeatPoint(Instance);
	}
	FORCEINLINE void UnregisterHeatPoint(AHeatPoint* Instance)
	{
		if (!Instance || !HeatPointmanager) { return; }
		HeatPointmanager->UnregisterHeatPoint(Instance);
	}

	float GetDistanceToNightstalker(const FVector& Location)
	{
		if (!Nightstalker) { return -1.0f; }

		return FVector::Dist(Location, Nightstalker->GetActorLocation());
	}

private:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

public:
	UFUNCTION(BlueprintGetter, Category = "Sensory Event Manager")
	FORCEINLINE USensoryEventManager* GetSensoryEventManager() const { return SensoryEventManager; }

	UFUNCTION(BlueprintGetter, Category = "Heat Point Manager")
	FORCEINLINE UHeatPointManager* GetHeatPointManager() const { return HeatPointmanager; }
};
