// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AmbiverseSoundDistributionData.h"
#include "AmbiverseSoundSource.h"
#include "AmbiverseElementAsset.generated.h"

class UAmbiverseDistributor;

/** An ambiverse element is a single procedural sound. It can be played directly, or used in a scene to create a procedural soundscape. */
UCLASS(Blueprintable, BlueprintType, ClassGroup = "Ambiverse", Meta = (DisplayName = "Ambiverse Element",
	ShortToolTip = "A single sound element that can be used inside an Ambiverse Layer"))
class AMBIVERSE_API UAmbiverseElementAsset : public UObject
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogAmbiverseElement, Log, All)

private:
	UPROPERTY(EditAnywhere, Category = "Sound")
	TSoftObjectPtr<USoundBase> Sound;
	
	/**
	 *  Custom SoundSource classes can be used to implement complex behavior for specific sounds.
	 *  You can create a blueprint derived from UAmbiverseSoundSource to script custom behavior.
	 *  @see AAmbiverseSoundSource.
	 *	@brief The soundsource class to use for this element.
	 */
	UPROPERTY(EditAnywhere, Category = "Advanced")
	TSubclassOf<AAmbiverseSoundSource> SoundSourceClass {AAmbiverseSoundSource::StaticClass()};

	/**
	 *  Custom distributors can be used to implement complex environment-aware placement behavior that is not available
	 *  in the default Ambiverse implementation. You can create a blueprint derived from UAmbiverseDistributor to script custom behavior.
	 *	Setting this property will override the distribution data, as the distributor will manage the placement behavior of this element instead.
	 *	@see UAmbiverseDistributorAsset.
	 *	@brief The distributor class to use for this element.
	 *	@note Setting this property causes the distribution data to be ignored and to become uneditable.
	 */
	UPROPERTY(EditAnywhere, Category = "Advanced", Meta = (BlueprintBaseOnly))
	TSubclassOf<UAmbiverseDistributor> DistributorClass;

public:
	bool IsValid {true};

	FORCEINLINE USoundBase* GetSound()
	{
		if (!Sound.IsValid()) { Sound.LoadSynchronous(); }
		return Sound.Get();
	}
	FORCEINLINE TSubclassOf<UAmbiverseDistributor> GetDistributorClass() const { return DistributorClass; }
	FORCEINLINE TSubclassOf<AAmbiverseSoundSource> GetSoundSourceClass() const { return SoundSourceClass; }
};
