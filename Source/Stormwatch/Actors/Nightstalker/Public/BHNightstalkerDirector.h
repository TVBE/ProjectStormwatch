// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BHHeatPointManager.h"
#include "BHNightstalker.h"
#include "Subsystems/WorldSubsystem.h"
#include "BHNightstalkerDirector.generated.h"

class USensoryEventManager;
class ABHNightstalker;
class AHeatPoint;

UCLASS(ClassGroup = "Nightstalker")
class STORMWATCH_API UBHNightstalkerDirector : public UWorldSubsystem
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogNightstalkerDirector, Log, All)

private:
	UPROPERTY(BlueprintGetter = GetSensoryEventManager)
	USensoryEventManager* SensoryEventManager;

	UPROPERTY(BlueprintGetter = GetHeatPointManager)
	UBHHeatPointManager* HeatPointmanager;

	UPROPERTY()
	ABHNightstalker* Nightstalker;

public:
	void RegisterNightstalker(ABHNightstalker* Instance);
	void UnregisterNightstalker(ABHNightstalker* Instance);
	
	FORCEINLINE void RegisterHeatPoint(ABHHeatPoint* Instance)
	{
		if (!Instance || !HeatPointmanager) { return; }
		HeatPointmanager->RegisterHeatPoint(Instance);
	}
	FORCEINLINE void UnregisterHeatPoint(ABHHeatPoint* Instance)
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

	UFUNCTION()
	void HandlePlayerPerceptionChanged(bool IsPlayerDetected);

public:
	UFUNCTION(BlueprintGetter, Category = "Sensory Event Manager")
	FORCEINLINE USensoryEventManager* GetSensoryEventManager() const { return SensoryEventManager; }

	UFUNCTION(BlueprintGetter, Category = "Heat Point Manager")
	FORCEINLINE UBHHeatPointManager* GetHeatPointManager() const { return HeatPointmanager; }

	FORCEINLINE ABHNightstalker* GetNightstalker() const { return Nightstalker; }
};
