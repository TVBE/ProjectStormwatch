// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#pragma once

#include "MetasoundSource.h"
#include "AmbiverseSoundSourceData.generated.h"

class UAmbiverseLayerAsset;

/** Contains data that can be used by an AmbienceSoundSource instance. */
USTRUCT(BlueprintType)
struct FAmbiverseSoundSourceData
{
	GENERATED_USTRUCT_BODY()
	
	/** The MetaSoundSource to use for a SoundSource. */
	UPROPERTY()
	UMetaSoundSource* Sound {nullptr};
	
	/** The transform to play a SoundSource at. */
	UPROPERTY()
	FTransform Transform {FTransform()};

	/** The name for the sound source. */
	UPROPERTY()
	FName Name {FName("None")};
	
	FAmbiverseSoundSourceData()
	{
	}
};
