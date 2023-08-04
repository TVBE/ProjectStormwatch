// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "AmbiverseSubsystem.generated.h"

class UAmbiverseSubsystemComponent;
class UAmbiverseSoundscapeManager;
class UAmbiverseElement;
class UAmbiverseDebugVisualizationComponent;
class UAmbiverseDistributionHandler;
class UAmbiverseLayerManager;
class UAmbiverseParameterManager;
class UAmbiverseSoundSourcePool;
class UAmbiverseLayerAsset;

UCLASS(Transient, ClassGroup = "Ambiverse", Meta = (DisplayName = "Ambiverse"))
class AMBIVERSE_API UAmbiverseSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogAmbiverseSubsystem, Log, All)

private:
	/** Array containing all subsystem components in this subsystem.
	 *	@note The elements in this array can be garbage collected when the
	 *	strong object pointers are reset or go out of scope. */
	TArray<UAmbiverseSubsystemComponent*> SubsystemComponents;

	TStrongObjectPtr<UAmbiverseSoundscapeManager> SoundscapeManager;
	TStrongObjectPtr<UAmbiverseParameterManager> ParameterManager;
	TStrongObjectPtr<UAmbiverseSoundSourcePool> SoundSourcePool;
	
	TStrongObjectPtr<UAmbiverseLayerManager> LayerManager; // TODO: remove.
	TStrongObjectPtr<UAmbiverseDistributionHandler> DistributionHandler; // TODO: remove.
	
#if !UE_BUILD_SHIPPING
	TStrongObjectPtr<UAmbiverseDebugVisualizationComponent> DebugComponent;
#endif

public:
	UAmbiverseSubsystem();
	
	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(UAmbiverseSubsystem, STATGROUP_Tickables);
	}

	/** Adds an ambiverse layer to the subsystem if the layer is not already active. */
	UFUNCTION(BlueprintCallable, Category = "Ambiverse", Meta = (DisplayName = "Add Layer"))
	bool AddAmbiverseLayer(UAmbiverseLayerAsset* Asset);
	
	/**
	 *	Removes an Ambiverse layer from the subsystem if the layer is currently active.
	 *	@param Asset The layer to remove.
	 *	@param ForceStop If true, all elements from the layer that are currently playing are stopped.
	 *	@param FadeTime An optional fade time for all elements to make the deactivation less noticable.
	 *	@return Whether the specified layer was found and removed succesfully. This will be true if some of the layer's elements are still playing.
	 */
	UFUNCTION(BlueprintCallable, Category = "Ambiverse", Meta = (DisplayName = "Remove Layer"))
	bool RemoveAmbiverseLayer(UAmbiverseLayerAsset* Asset, const bool ForceStop = false, const float FadeTime = 1.0f);

private:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	
	virtual void Tick(float DeltaTime) override;

public:
	FORCEINLINE UAmbiverseLayerManager* GetLayerManager() const { return LayerManager.Get(); }
	FORCEINLINE UAmbiverseParameterManager* GetParameterManager() const { return ParameterManager.Get(); }
	FORCEINLINE UAmbiverseSoundSourcePool* GetSoundSourcePool() const { return SoundSourcePool.Get(); }
	FORCEINLINE UAmbiverseDistributionHandler* GetDistributionManager() const { return DistributionHandler.Get(); }
	FORCEINLINE UAmbiverseSoundscapeManager* GetSoundscapeManager() const { return SoundscapeManager.Get(); }
};



