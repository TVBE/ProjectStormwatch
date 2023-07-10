// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AmbiverseLayerAsset.h"
#include "AmbiverseSubsystemComponent.h"
#include "AmbiverseLayerManager.generated.h"

class UAmbiverseLayerInstance;
class UAmbiverseCompositeAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLayerRegisteredDelegate, UAmbiverseLayerAsset*, RegisteredLayer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLayerUnregisteredDelegate, UAmbiverseLayerAsset*, UnregisteredLayer);

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
	TArray<UAmbiverseLayerInstance*> ActiveLayers;

public:
	virtual void Initialize(UAmbiverseSubsystem* Subsystem) override;
	virtual void Deinitialize(UAmbiverseSubsystem* Subsystem) override;

	virtual void Tick(const float DeltaTime) override;
	
	bool RegisterAmbiverseLayer(UAmbiverseLayerAsset* LayerAsset);
	bool UnregisterAmbiverseLayer(UAmbiverseLayerAsset* LayerAsset, const bool ForceStop = false, const float FadeTime = 1.0f);

	// TODO: Reimplement composites.
	// void RegisterAmbiverseComposite(UAmbiverseCompositeAsset* Composite);
	// void UnregisterAmbiverseComposite(UAmbiverseCompositeAsset* Composite);
	
	/** Tries to find a layer instance by the asset that is provided. */
	UAmbiverseLayerInstance* FindInstanceByAsset(UAmbiverseLayerAsset* Asset) const;

private:
	UFUNCTION()
	void HandleOnParameterChanged(UAmbiverseParameterAsset* ChangedParameter);

public:
	FORCEINLINE TArray<UAmbiverseLayerInstance*> GetLayerRegistry() const { return ActiveLayers; }
};
