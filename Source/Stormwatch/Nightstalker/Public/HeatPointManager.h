// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Stormwatch

#pragma once

#include "CoreMinimal.h"
#include "SensoryEventManager.h"
#include "HeatPointManager.generated.h"

class AHeatPoint;
class UNightstalkerDirector;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHottestHeatPointChangedDelegate, AHeatPoint*, HeatPoint);

UCLASS()
class STORMWATCH_API UHeatPointManager : public UObject
{
	GENERATED_BODY()
	
	DECLARE_LOG_CATEGORY_CLASS(LogHeatPointManager, Log, All)

public:
#if WITH_EDITORONLY_DATA
	bool IsDebugVisualiationEnabled {true};
#endif

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnHottestHeatPointChangedDelegate OnHottestHeatPointChanged;

private:
	/** Pointer to the subsystem that owns this object. */
	UPROPERTY()
	UNightstalkerDirector* Director {nullptr};
	
	UPROPERTY()
	TArray<AHeatPoint*> HeatPoints;
	
	UPROPERTY()
	FTimerHandle HeatPointProcessorTimerHandle;

public:
	UHeatPointManager();

	void Initialize(UNightstalkerDirector* Subsystem);
	void Deinitialize();

	void Activate();
	void Deactivate();

	/** Registers a heat point instance to the heat point manager. */
	void RegisterHeatPoint(AHeatPoint* Instance);

	/** Registers a heat point instance from the heat point manager.
	 *	The lifetime of a heat point is managed by the heat point manager, so this function should only be called in special occasions. */
	void UnregisterHeatPoint(AHeatPoint* Instance);

	/** Updates the size and heat of existing heat points. */
	void UpdateHeatPoints(TArray<FHeatPointOverlapData>& OverlapData);

	/** Sorts the heat points array by heat value in ascending order. */
	void SortHeatPointsByHeatValue();

	/** Destroys all active heat points. */
	void FlushHeatPoints();

	bool IsActive() const;

	UFUNCTION(BlueprintPure)
	AHeatPoint* GetHottestHeatPoint();

private:
	/** Updates the lifetime of every heat point instance in the world. */
	UFUNCTION()
	void UpdateHeatPointLifeTime();
	
	void RemoveZeroHeatPoints();

public:
	FORCEINLINE TArray<AHeatPoint*> GetHeatPoints() const { return HeatPoints; }
};

USTRUCT()
struct FHeatPointOverlapData
{
	GENERATED_BODY()
	
	UPROPERTY()
	AHeatPoint* HeatPoint {nullptr};

	UPROPERTY()
	FHeatEvent Event;
	
	FHeatPointOverlapData()
	{
	}

	FHeatPointOverlapData(AHeatPoint* InHeatPoint, const FHeatEvent& InEvent)
		: HeatPoint(InHeatPoint), Event(InEvent)
	{
	}
};
