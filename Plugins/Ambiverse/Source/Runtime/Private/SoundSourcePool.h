// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SoundSourceData.h"
#include "SubsystemComponent.h"
#include "SoundSourcePool.generated.h"

class UAmbiverseElement;
class AAmbiverseSoundSource;

UCLASS()
class AMBIVERSE_API USoundSourcePool : public USubsystemComponent
{
	GENERATED_BODY()

private:
	/** Pool of SoundSource instances. These can include subobjects. */
	UPROPERTY(Transient)
	TArray<AAmbiverseSoundSource*> Pool;

	/** Array of sound sources that are currently playing. */
	UPROPERTY(Transient)
	TArray<AAmbiverseSoundSource*> ActiveSoundSources;

public:
	virtual bool IsTickable() const override { return false; }
	
	void InitiateSoundSource(FSoundSourceData& SoundSourceData, UAmbiverseElement* ElementInstance);

	void HandleSoundSourceFinishedPlayback(AAmbiverseSoundSource* SoundSource);

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
