// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AmbiverseElement.h"
#include "Components/AudioComponent.h"
#include "GameFramework/Actor.h"
#include "AmbiverseSoundSource.generated.h"

class UAmbiverseSceneAsset;
class USoundSourcePool;

/** Contains data that can be used by an AmbienceSoundSource instance. */
USTRUCT()
struct FSoundSourceData
{
	GENERATED_USTRUCT_BODY()
	
	/** The element that this sound source is currently playing. */
	UPROPERTY()
	FAmbiverseElement Element;
	
	/** The transform to play a SoundSource at. */
	UPROPERTY()
	FTransform Transform;
	
	FSoundSourceData(
	const FAmbiverseElement&	InElement,
	const FTransform&			InTransform
	)
	: Element(InElement)
	, Transform(InTransform)
	{}
};

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
	
	UPROPERTY(BlueprintGetter = GetSoundSourceData)
	FSoundSourceData SoundSourceData;

public:	
	AAmbiverseSoundSource();

	/** Initializes the sound source. If initialization was succesful, this will immediatly start playing back the sound. */
	bool Initialize(FSoundSourceData& Data, UAmbiverseElement* ElementInstance);

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

	UFUNCTION(BlueprintImplementableEvent, Meta = (DisplayName = "Begin Playback"))
	void OnBeginPlayback();
	
	UFUNCTION(BlueprintImplementableEvent, Meta = (DisplayName = "End Playback"))
	void OnEndPlayback();
	
	UFUNCTION(BlueprintGetter)
	FORCEINLINE UAudioComponent* GetAudioComponent() const { return AudioComponent; }

	UFUNCTION(BlueprintGetter)
	FORCEINLINE FSoundSourceData GetSoundSourceData() const { return SoundSourceData; }

private:
	UFUNCTION()
	void HandleOnAudioFinishedPlaying();
};
