// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "SensoryEvents.h"
#include "UObject/NoExportTypes.h"
#include "SensoryEventManager.generated.h"

class UNightstalkerDirector;
class ANightstalker;

UCLASS()
class USensoryEventManager : public UObject
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogSensoryEventManager, Log, All)

public:
	UPROPERTY()
	ANightstalker* Nightstalker;

private:
	/** Pointer to the subsystem that owns this object. */
	UPROPERTY()
	UNightstalkerDirector* Director {nullptr};
	
	/** Array of auditory events that are waiting to be processed. */
	UPROPERTY()
	TArray<FAuditoryEvent> AuditoryEventQueue;

	/** Timer handle for the auditory event processor. */
	FTimerHandle AuditoryEventProcessorTimerHandle;

public:
	void Initialize(UNightstalkerDirector* Subsystem);
	void Deinitialize();
	
	void AddAuditoryEventAtLocation(FAuditoryEvent Event, const FVector& Location);

private:
	UFUNCTION()
	void ProcessAuditoryEvents();
};

USTRUCT()
struct FHeatEvent
{
	GENERATED_BODY()
	
	float Heat;

	float Radius;
	
	FVector Location;
	
	FHeatEvent()
		: Heat(0.0f)
		, Radius(0.0f)
		, Location(FVector::ZeroVector)
	{
	}

	FHeatEvent(float InHeat, float InRadius, const FVector& InLocation)
		: Heat(InHeat)
		, Radius(InRadius)
		, Location(InLocation)
	{
	}
};
