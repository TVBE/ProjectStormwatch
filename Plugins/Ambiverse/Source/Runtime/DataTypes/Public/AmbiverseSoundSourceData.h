// Copyright (c) 2022-present Tim Verberne
// This source code is part of the Adaptive Ambience System plugin

#pragma once

#include "MetasoundSource.h"
#include "AmbiverseSoundSourceData.generated.h"

class UAmbiverseLayer;

/** Contains data that can be used by an AmbienceSoundSource instance. */
USTRUCT(BlueprintType)
struct FAmbiverseSoundSourceData
{
	GENERATED_USTRUCT_BODY()
	
	/** The MetaSoundSource to use for an AmbienceSoundSource. */
	UPROPERTY()
	UMetaSoundSource* Sound {nullptr};
	
	/** The volume to play an AmbienceSoundSource at. */
	UPROPERTY()
	float Volume {1.0f};
	
	/** The transform to play an AmbienceSoundSource at. */
	UPROPERTY()
	FTransform Transform {FTransform()};

	/** The name for the sound source. */
	UPROPERTY()
	FName Name {FName("None")};

	/** The ambiverse layer responsible for initializing the soundsource. */
	UPROPERTY()
	UAmbiverseLayer* Layer {nullptr};
	
	/** Constructor with default values. */
	FAmbiverseSoundSourceData()
	{
	}

};
