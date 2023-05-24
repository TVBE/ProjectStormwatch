// Copyright (c) 2023-present Tim Verberne
// This source code is part of the Ambiverse plugin

#pragma once

#include "CoreMinimal.h"
#include "AmbiverseLayer.h"
#include "AmbiverseSubsystemComponent.h"
#include "AmbiverseLayerManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLayerRegisteredDelegate, UAmbiverseLayer*, RegisteredLayer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLayerUnregisteredDelegate, UAmbiverseLayer*, UnregisteredLayer);

UCLASS()
class UAmbiverseLayerManager : public UAmbiverseSubsystemComponent
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogAmbiverseLayerManager, Log, All)

public:
	UPROPERTY()
	FOnLayerRegisteredDelegate OnLayerRegistered;

	UPROPERTY()
	FOnLayerRegisteredDelegate OnLayerUnregistered;

private:
	/** The current active ambience layers. */
	UPROPERTY()
	TArray<UAmbiverseLayer*> LayerRegistry;

public:
	virtual void Initialize(UAmbiverseSubsystem* Subsystem) override;
	virtual void Deinitialize(UAmbiverseSubsystem* Subsystem) override;
	
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
