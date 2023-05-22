// Copyright (c) 2022-present Tim Verberne
// This source code is part of the Adaptive Ambience System plugin

#pragma once

#include "CoreMinimal.h"
#include "AmbiverseLayer.h"
#include "AmbiverseLayerManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLayerRegisteredDelegate, UAmbiverseLayer*, RegisteredLayer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLayerUnregisteredDelegate, UAmbiverseLayer*, UnregisteredLayer);

UCLASS()
class UAmbiverseLayerManager : public UObject
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogAmbiverseLayerManager, Log, All)

public:
	UPROPERTY()
	FOnLayerRegisteredDelegate OnLayerRegistered;

	UPROPERTY()
	FOnLayerRegisteredDelegate OnLayerUnregistered;

private:
	/** Pointer to the Ambiverse subsystem. */
	UPROPERTY()
	UAmbiverseSubsystem* AmbiverseSubsystem;
	
	/** The current active ambience layers. */
	UPROPERTY()
	TArray<UAmbiverseLayer*> LayerRegistry;

public:
	void Initialize(UAmbiverseSubsystem* Subsystem);
	void Deinitialize();
	
	void RegisterAmbiverseLayer(UAmbiverseLayer* Layer);
	void UnregisterAmbiverseLayer(UAmbiverseLayer* Layer);
	
	/** Checks if an ambience layer is already active*/
	UAmbiverseLayer* FindActiveAmbienceLayer(const UAmbiverseLayer* LayerToFind) const;

private:
	UFUNCTION()
	void HandleOnParameterChanged(UAmbiverseParameter* ChangedParameter);

public:
	FORCEINLINE TArray<UAmbiverseLayer*> GetLayerRegistry() const { return LayerRegistry; }
	
};
