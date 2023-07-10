// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AmbiverseSubsystemComponent.h"
#include "AmbiverseElementManager.generated.h"

class UAmbiverseElementInstance;

UCLASS()
class UAmbiverseElementManager : public UAmbiverseSubsystemComponent
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogAmbiverseElementManager, Log, All)

private:
	UPROPERTY()
	TArray<UAmbiverseElementInstance*> ScheduledElementInstances;

	UPROPERTY()
	TArray<UAmbiverseElementInstance*> PlayingProceduralElements;

public:
	virtual void Tick(const float DeltaTime) override;

	void RegisterElements(TArray<UAmbiverseElementInstance*> Elements);
	void UnregisterElements(TArray<UAmbiverseElementInstance*> Elements);

	/** Determines if a finished element should be rescheduled if it is still valid and not already scheduled.
	 *	Situations where this might not be the case is if the element is marked PendingKill, or its IntervalMode is set to OnSpawn.
	 *	This is basically a handover function that is exclusively called by the SoundSourceManager within the owner subsystem. */
	void EvaluateFinishedElement(UAmbiverseElementInstance* Element);

private:
	void ScheduleProceduralElement(UAmbiverseElementInstance* ElementInstance, const bool IgnoreMin = false);
};


