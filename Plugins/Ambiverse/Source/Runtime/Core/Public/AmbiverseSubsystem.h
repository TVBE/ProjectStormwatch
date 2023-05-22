// Copyright (c) 2022-present Tim Verberne
// This source code is part of the Adaptive Ambience System plugin

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "AmbiverseSubsystem.generated.h"

class UAmbiverseSoundSourceManager;
class UAmbiverseLayer;

UCLASS(ClassGroup = "Ambiverse")
class AMBIVERSE_API UAmbiverseSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogAmbiverseSubsystem, Log, All)

public:
#if !UE_BUILD_SHIPPING
	TUniquePtr<FAutoConsoleCommand> SoundSourceVisualisationConsoleCommand;
#endif

private:
	/** The sound source manager object. */
	UPROPERTY()
	UAmbiverseSoundSourceManager* SoundSourceManager {nullptr};
	
	/** The current active ambience layers. */
	UPROPERTY()
	TArray<UAmbiverseLayer*> ActiveLayers;

public:
	/** Adds a sound set*/
	void AddAmbienceLayer(UAmbiverseLayer* Layer);
	void PopAmbienceLayer(UAmbiverseLayer* Layer);

private:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Deinitialize() override;
	
	/** Processes an ambience event and updates the queue for an ambience layer.*/
	UFUNCTION()
	void ProcessAmbienceLayerQueue(UAmbiverseLayer* Layer,  FAmbiverseLayerQueueEntry& Entry);

	/** Checks if an ambience layer*/
	UAmbiverseLayer* FindActiveAmbienceLayer(const UAmbiverseLayer* LayerToFind) const;

	static float GetSoundInterval(const UAmbiverseLayer* Layer, const FAmbiverseLayerQueueEntry& Entry);
	static float GetSoundVolume(const UAmbiverseLayer* Layer, const FAmbiverseLayerQueueEntry& Entry);

#if !UE_BUILD_SHIPPING
	void SetSoundSourceVisualisationEnabled(bool IsEnabled);
#endif
};



