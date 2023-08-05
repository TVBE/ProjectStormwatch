// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AmbiverseScene.h"
#include "AmbiverseSubsystemComponent.h"
#include "AmbiverseSoundscapeManager.generated.h"

class UAmbiverseElement;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLayerRegisteredDelegate, UAmbiverseSceneAsset*, RegisteredLayer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLayerUnregisteredDelegate, UAmbiverseSceneAsset*, UnregisteredLayer);

UCLASS()
class UAmbiverseSoundscapeManager : public UAmbiverseSubsystemComponent
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FOnLayerRegisteredDelegate OnLayerRegistered;

	UPROPERTY()
	FOnLayerRegisteredDelegate OnLayerUnregistered;

private:
	/** The current active ambience layers. */
	UPROPERTY()
	TArray<FAmbiverseScene> Scenes;

public:
	virtual void Initialize(UAmbiverseSubsystem* Subsystem) override;
	virtual void Deinitialize(UAmbiverseSubsystem* Subsystem) override;
	
	virtual void Tick(const float DeltaTime) override;

	bool RegisterScene(UAmbiverseSceneAsset* LayerAsset);
	bool UnregisterScene(UAmbiverseSceneAsset* LayerAsset, const bool ForceStop = false, const float FadeTime = 1.0f);

	void RegisterElements(TArray<UAmbiverseElement*> Elements);
	void UnregisterElements(TArray<UAmbiverseElement*> Elements);

	/** Determines if a finished element should be rescheduled if it is still valid and not already scheduled.
	 *	Situations where this might not be the case is if the element is marked PendingKill, or its IntervalMode is set to OnSpawn.
	 *	This is basically a handover function that is exclusively called by the SoundSourceManager within the owner subsystem. */
	void EvaluateFinishedElement(UAmbiverseElement* Element);

	// TODO: Reimplement composites.
	// void RegisterAmbiverseComposite(UAmbiverseCompositeAsset* Composite);
	// void UnregisterAmbiverseComposite(UAmbiverseCompositeAsset* Composite);
	
	/** Tries to find a layer instance by the asset that is provided. */
	UAmbiverseLayer* FindSceneByAsset(UAmbiverseSceneAsset* Asset) const;

private:
	void ScheduleProceduralElement(UAmbiverseElement* ElementInstance, const bool IgnoreMin = false);

public:
	FORCEINLINE TArray<UAmbiverseLayer*> GetActiveLayers() const { return Scenes; }
};


