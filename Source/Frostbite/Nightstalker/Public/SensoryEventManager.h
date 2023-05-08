// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "SensoryEvents.h"
#include "UObject/NoExportTypes.h"
#include "SensoryEventManager.generated.h"

USTRUCT()
struct FHeatAtLocation
{
	GENERATED_BODY()
	
	float Heat;
	
	FVector Location;
	
	FHeatAtLocation()
		: Heat(0.0f)
		, Location(FVector::ZeroVector)
	{
	}
	
	FHeatAtLocation(float InHeat, FVector InLocation)
		: Heat(InHeat)
		, Location(InLocation)
	{
	}
};

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
	/** Array of auditory events that are waiting to be processed. */
	UPROPERTY()
	TArray<FAuditoryEvent> AuditoryEventQueue;

	/** Timer handle for the auditory event processor. */
	FTimerHandle AuditoryEventProcessorTimerHandle;

public:
	void Initialize();
	
	void AddAuditoryEventAtLocation(FAuditoryEvent Event, const FVector& Location);

private:
	
	UFUNCTION()
	void ProcessAuditoryEvents();
};
