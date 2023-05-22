// Copyright (c) 2022-present Tim Verberne
// This source code is part of the Adaptive Ambience System plugin

#pragma once

#include "MetasoundSource.h"
#include "AmbiverseSoundDistributionData.h"
#include "AmbiverseParameter.h"
#include "AmbiverseProceduralSoundData.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogProceduralSoundData, Log, All)

/** Defines the spawning behavior of a spatial AmbienceSoundSource */
USTRUCT(BlueprintType)
struct FAmbiverseProceduralSoundData
{
	GENERATED_USTRUCT_BODY()
	
	/** The name of an AmbienceSystem preset entry. Is only used for debugging purposes. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Data")
	FName Name;
	
	/** The MetaSoundSource types and their corresponding weights.
	If the TMap is empty or the weights are set improperly, an even distribution of selection for the sounds will be used. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
	TMap<UMetaSoundSource*, int> Sounds;

	/** The volume multiplier for an AmbienceSystem preset entry */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Data",
		Meta = (ClampMin = "0", ClampMax = "2", UIMin = "0", UIMax = "2"))
	float Volume {1.0f};
	
	/** The minimum delay between instances of an AmbienceSystem preset entry occuring. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Data")
	float DelayMin {10.0f};
	
	/** The maximum delay between instances of an AmbienceSystem preset entry occuring. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Data")
	float DelayMax {30.0f};
	
	/** PlayRange data for an AmbienceSystem preset entry. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Data")
	FAmbiverseSoundDistributionData DistributionData;

	/** Parameters that influence all procedural sounds in this layer. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parameters")
	TArray<FAmbiverseParameterModifiers> Parameters;
	
	/** When set to false, this AmbienceSystem preset entry will not play. Useful to temporarily disable preset entries without deleting them. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Data")
	bool Use {true};

	bool IsValid {true};

	static bool Validate(FAmbiverseProceduralSoundData& SoundData);

	static UMetaSoundSource* GetSoundFromMap(const TMap<UMetaSoundSource*, int>& SoundMap);
};


