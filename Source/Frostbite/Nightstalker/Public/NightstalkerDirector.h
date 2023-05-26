// Copyright (c) 2022-present Barrelhouse. All rights reserved.
// Written by Tim Verberne.

#pragma once

#include "CoreMinimal.h"
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

	UPROPERTY()
	ANightstalker* Nightstalker;
	
	UPROPERTY(BlueprintGetter = GetHeatPoints)
	TArray<AHeatPoint*> HeatPoints;

public:
	void RegisterNightstalker(ANightstalker* Instance);
	void UnregisterNightstalker(ANightstalker* Instance);
	
	void RegisterHeatPoint(AHeatPoint* Instance);
	void UnregisterHeatPoint(AHeatPoint* Instance);

	UFUNCTION(BlueprintGetter, Category = "Sensory Event Manager")
	FORCEINLINE USensoryEventManager* GetSensoryEventManager() const { return SensoryEventManager; }
	
	UFUNCTION(BlueprintGetter)
	FORCEINLINE TArray<AHeatPoint*> GetHeatPoints() const { return HeatPoints; }

private:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
};
