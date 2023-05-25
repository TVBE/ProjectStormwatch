// Copyright (c) 2023-present Tim Verberne
// This source code is part of the Ambiverse plugin

#pragma once

#include "MetasoundSource.h"
#include "AmbiverseSoundDistributionData.h"
#include "AmbiverseParameter.h"
#include "AmbiverseProceduralSoundData.generated.h"

class UAmbiverseElement;

DECLARE_LOG_CATEGORY_EXTERN(LogProceduralSoundData, Log, All)

/** Defines the spawning behavior of a spatial AmbienceSoundSource */
USTRUCT(BlueprintType)
struct FAmbiverseProceduralElementData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	UAmbiverseElement* Element {nullptr};
	
	/** The minimum delay between instances of an AmbienceSystem preset entry occuring. */
	UPROPERTY(EditAnywhere)
	float DelayMin {10.0f};
	
	/** The maximum delay between instances of an AmbienceSystem preset entry occuring. */
	UPROPERTY(EditAnywhere)
	float DelayMax {30.0f};
};


