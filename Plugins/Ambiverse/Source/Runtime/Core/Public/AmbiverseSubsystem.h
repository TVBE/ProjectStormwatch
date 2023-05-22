// Copyright (c) 2022-present Tim Verberne
// This source code is part of the Adaptive Ambience System plugin

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "AmbiverseSubsystem.generated.h"

class UAmbiverseLayerManager;
class UAmbiverseParameterManager;
class UAmbiverseSoundSourceManager;
class UAmbiverseLayer;

UCLASS(ClassGroup = "Ambiverse")
class AMBIVERSE_API UAmbiverseSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogAmbiverseSubsystem, Log, All)

public:
#if !UE_BUILD_SHIPPING
	TUniquePtr<FAutoConsoleCommand> SoundSourceVisualisationConsoleCommand;
#endif

private:
	/** The layer manager object. */
	UPROPERTY()
	UAmbiverseLayerManager* LayerManager {nullptr};
	
	/** The sound source manager object. */
	UPROPERTY()
	UAmbiverseSoundSourceManager* SoundSourceManager {nullptr};

	/** The parameter manager object. */
	UPROPERTY()
	UAmbiverseParameterManager* ParameterManager {nullptr};

public:
	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(UAmbiverseSubsystem, STATGROUP_Tickables);
	}

private:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Deinitialize() override;

	virtual void Tick(float DeltaTime) override;

	/** Updates the remaining time of all sounds in the sound queue. */
	UFUNCTION()
	void UpdateActiveLayers(const float DeltaTime);
	
	/** Processes an ambience event and updates the queue for an ambience layer.*/
	UFUNCTION()
	void ProcessAmbienceLayerQueue(UAmbiverseLayer* Layer,  FAmbiverseLayerQueueEntry& Entry);
	
	static float GetSoundInterval(const UAmbiverseLayer* Layer, const FAmbiverseLayerQueueEntry& Entry);
	static float GetSoundVolume(const UAmbiverseLayer* Layer, const FAmbiverseLayerQueueEntry& Entry);

	UFUNCTION()
	void HandleParameterChanged();

#if !UE_BUILD_SHIPPING
	void SetSoundSourceVisualisationEnabled(bool IsEnabled);
#endif

public:
	FORCEINLINE UAmbiverseLayerManager* GetLayerManager() const { return LayerManager; }
	FORCEINLINE UAmbiverseSoundSourceManager* GetSoundSourceManager() const { return SoundSourceManager; }
	FORCEINLINE UAmbiverseParameterManager* GetParameterManager() const { return ParameterManager; }
};



