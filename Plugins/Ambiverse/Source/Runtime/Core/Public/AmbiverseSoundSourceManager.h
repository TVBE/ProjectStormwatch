// Copyright (c) 2022-present Tim Verberne
// This source code is part of the Adaptive Ambience System plugin

#pragma once

#include "CoreMinimal.h"
#include "AmbiverseSoundSourceData.h"
#include "AmbiverseSoundSourceManager.generated.h"

class AAmbiverseSoundSource;

UCLASS(Blueprintable, BlueprintType, ClassGroup = "Ambiverse")
class AMBIVERSE_API UAmbiverseSoundSourceManager : public UObject
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
	void Initialize();
	void Deinitialize();
	
	void InitiateSoundSource(FAmbiverseSoundSourceData& SoundSourceData);

	UFUNCTION(BlueprintCallable)
	void ReleaseToPool(AAmbiverseSoundSource* SoundSource);

#if !UE_BUILD_SHIPPING
	void SetSoundSourceVisualisationEnabled(const bool IsEnabled);
#endif
};
