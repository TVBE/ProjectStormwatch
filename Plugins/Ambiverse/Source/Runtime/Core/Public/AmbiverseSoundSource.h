// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AmbiverseSoundSourceData.h"
#include "Components/AudioComponent.h"
#include "GameFramework/Actor.h"
#include "AmbiverseSoundSource.generated.h"

class UAmbiverseElementInstance;
class UAmbiverseLayerAsset;
class UAmbiverseSoundSourceManager;

DECLARE_DELEGATE_OneParam(FOnFinishedPlaybackDelegate, AAmbiverseSoundSource*)

UCLASS(Blueprintable, BlueprintType, NotPlaceable, ClassGroup = "Ambiverse")
class AMBIVERSE_API AAmbiverseSoundSource : public AActor
{
	GENERATED_BODY()
	
	DECLARE_LOG_CATEGORY_CLASS(LogAmbiverseSoundSource, Log, All)

public:
#if !UE_BUILD_SHIPPING
	bool IsDebugVisualisationEnabled {false};
	float ActiveTime {0.0f};
#endif

	FOnFinishedPlaybackDelegate OnFinishedPlayback;

private:
	/** The audio component to play back sounds at. */
	UPROPERTY(BlueprintGetter = GetAudioComponent)
	UAudioComponent* AudioComponent;

	/** Pointer to the sound source manager. */
	UPROPERTY(Transient)
	UAmbiverseSoundSourceManager* SoundSourceManager;
	
	UPROPERTY(BlueprintGetter = GetSoundSourceData)
	FAmbiverseSoundSourceData SoundSourceData;

	/** The element that this sound source is currently playing. */
	UPROPERTY(Transient)
	UAmbiverseElementInstance* AssociatedElement;
	
public:	
	AAmbiverseSoundSource();

	/** Initializes the sound source. If initialization was succesful, this will immediatly start playing back the sound. */
	bool Initialize(FAmbiverseSoundSourceData& Data, UAmbiverseElementInstance* ElementInstance);

	/** Deinitializes the sound source and clears all data associated with the element the SoundSource was playing before.
	 *	This prevents lingering references to elements that might or might not exist anymore while this SoundSource is in the pool. */
	bool Deinitialize();
	
	virtual void Tick(float DeltaTime) override;

	void SetSound(USoundBase* NewSound)
	{
		if (!AudioComponent || !NewSound) { return; }
		AudioComponent->SetSound(NewSound);
	}

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	void BeginPlayback();
	
	UFUNCTION(BlueprintImplementableEvent)
	void EndPlayback();
	
	UFUNCTION(BlueprintGetter)
	FORCEINLINE UAudioComponent* GetAudioComponent() const { return AudioComponent; }

	UFUNCTION(BlueprintGetter)
	FORCEINLINE FAmbiverseSoundSourceData GetSoundSourceData() const { return SoundSourceData; }

private:
	UFUNCTION()
	void HandleOnAudioFinishedPlaying();

public:
	FORCEINLINE UAmbiverseElementInstance* GetAssociatedElement() const { return AssociatedElement; }
};
