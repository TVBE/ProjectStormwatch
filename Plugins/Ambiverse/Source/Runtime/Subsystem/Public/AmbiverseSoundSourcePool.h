// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AmbiverseSoundSourceData.h"
#include "AmbiverseSubsystemComponent.h"
#include "AmbiverseSoundSourcePool.generated.h"

class UAmbiverseElement;
class AAmbiverseSoundSource;

UCLASS()
class UAmbiverseSoundSourcePool : public UAmbiverseSubsystemComponent
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogAmbiverseSoundSourceManager, Log, All)

private:
	/** Pool of SoundSource instances. These can include subobjects. */
	UPROPERTY(Transient)
	TArray<AAmbiverseSoundSource*> Pool;

	/** Array of sound sources that are currently playing. */
	UPROPERTY(Transient)
	TArray<AAmbiverseSoundSource*> ActiveSoundSources;

#if !UE_BUILD_SHIPPING
	bool EnableSoundSourceVisualisation {false};
#endif

public:
	/** Initiates and plays an element. */
	void PlayElement(UAmbiverseElement* ElementInstance);
	
	void InitiateSoundSource(FAmbiverseSoundSourceData& SoundSourceData, UAmbiverseElement* ElementInstance);

	void HandleSoundSourceFinishedPlayback(AAmbiverseSoundSource* SoundSource);

#if !UE_BUILD_SHIPPING
	void SetSoundSourceVisualisationEnabled(const bool IsEnabled);
#endif

	private:
	/**
	 * Gets a AAmbiverseSoundSource instance of the specified class from the pool.
	 * @param Class The class of AmbiverseSoundSource to get.
	 * @return An instance of the specified class.
	 */
	AAmbiverseSoundSource* GetSoundSourceFromPool(TSubclassOf<AAmbiverseSoundSource> Class);

public:
	FORCEINLINE TArray<AAmbiverseSoundSource*> GetActiveSoundSources() const { return ActiveSoundSources; }
};
