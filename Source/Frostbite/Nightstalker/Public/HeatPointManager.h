// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "HeatPointManager.generated.h"

class AHeatPoint;
class UNightstalkerDirector;

UCLASS()
class UHeatPointManager : public UObject
{
	GENERATED_BODY()
	
	DECLARE_LOG_CATEGORY_CLASS(LogHeatPointManager, Log, All)

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

	/** Registers a heat point instance to the heat point manager. */
	void RegisterHeatPoint(AHeatPoint* Instance);

	/** Registers a heat point instance from the heat point manager.
	 *	The lifetime of a heat point is managed by the heat point manager, so this function should only be called in special occasions. */
	void UnregisterHeatPoint(AHeatPoint* Instance);

	/** Updates the lifetime of every heat point instance in the world. */
	UFUNCTION()
	void ProcessHeatPoints();

	FORCEINLINE TArray<AHeatPoint*> GetHeatpoints() const { return HeatPoints; }
	
};
