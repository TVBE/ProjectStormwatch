// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Stormwatch

#pragma once

#include "CoreMinimal.h"
#include "SensoryEventManager.h"
#include "BHHeatPointManager.generated.h"

class ABHHeatPoint;
class UBHNightstalkerDirector;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHottestHeatPointChangedDelegate, ABHHeatPoint*, HeatPoint);

UCLASS()
class STORMWATCH_API UBHHeatPointManager : public UObject
{
	GENERATED_BODY()
	
	DECLARE_LOG_CATEGORY_CLASS(LogHeatPointManager, Log, All)

public:
#if WITH_EDITORONLY_DATA
	bool IsDebugVisualiationEnabled = true;
#endif

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnHottestHeatPointChangedDelegate OnHottestHeatPointChanged;

private:
	/** Pointer to the subsystem that owns this object. */
	UPROPERTY()
	UBHNightstalkerDirector* Director = nullptr;
	
	UPROPERTY()
	TArray<ABHHeatPoint*> HeatPoints;
	
	UPROPERTY()
	FTimerHandle HeatPointProcessorTimerHandle;

public:
	UBHHeatPointManager();

	void Initialize(UBHNightstalkerDirector* Subsystem);
	void Deinitialize();

	void Activate();
	void Deactivate();

	/** Registers a heat point instance to the heat point manager. */
	void RegisterHeatPoint(ABHHeatPoint* Instance);

	/** Registers a heat point instance from the heat point manager.
	 *	The lifetime of a heat point is managed by the heat point manager, so this function should only be called in special occasions. */
	void UnregisterHeatPoint(ABHHeatPoint* Instance);

	/** Updates the size and heat of existing heat points. */
	void UpdateHeatPoints(TArray<FHeatPointOverlapData>& OverlapData);

	/** Sorts the heat points array by heat value in ascending order. */
	void SortHeatPointsByHeatValue();

	/** Destroys all active heat points. */
	void FlushHeatPoints();

	bool IsActive() const;

	UFUNCTION(BlueprintPure)
	ABHHeatPoint* GetHottestHeatPoint();

private:
	/** Updates the lifetime of every heat point instance in the world. */
	UFUNCTION()
	void UpdateHeatPointLifeTime();
	
	void RemoveZeroHeatPoints();

public:
	FORCEINLINE TArray<ABHHeatPoint*> GetHeatPoints() const { return HeatPoints; }
};

USTRUCT()
struct FHeatPointOverlapData
{
	GENERATED_BODY()
	
	UPROPERTY()
	ABHHeatPoint* HeatPoint = nullptr;

	UPROPERTY()
	FHeatEvent Event;
	
	FHeatPointOverlapData()
	{
	}

	FHeatPointOverlapData(ABHHeatPoint* InHeatPoint, const FHeatEvent& InEvent)
		: HeatPoint(InHeatPoint), Event(InEvent)
	{
	}
};
