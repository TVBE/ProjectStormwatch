// Copyright (c) 2022-present Tim Verberne
// This source code is part of the Adaptive Ambience System plugin

#pragma once

#include "CoreMinimal.h"
#include "AmbiverseParameter.h"
#include "AmbiverseProceduralSoundData.h"
#include "TimerManager.h"
#include "AmbiverseLayer.generated.h"

class UAmbiverseSubsystem;
class UAmbiverseParameter;
class UAmbiverseSoundSourceManager;

USTRUCT()
struct FAmbiverseLayerQueueEntry
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY()
	FAmbiverseProceduralSoundData SoundData {FAmbiverseProceduralSoundData()};

	UPROPERTY()
	float Time {0.0f};
};

UCLASS(Blueprintable, BlueprintType, ClassGroup = "Ambiverse", Meta = (DisplayName = "Ambiverse Layer"))
class AMBIVERSE_API UAmbiverseLayer : public UObject
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogAmbienceLayer, Log, All)

public:
#if WITH_EDITORONLY_DATA
	/** User friendly name for the layer. */
	UPROPERTY(EditAnywhere, Category = "Layer", Meta = (EditCondition = "IsEnabled"))
	FName Name;

	/** A description for the layer. */
	UPROPERTY(EditAnywhere, Category = "Layer", Meta = (EditCondition = "IsEnabled", MultiLine = "true"))
	FText Description;
#endif
	
	/** The color of the layer when visualised in the editor. */
	UPROPERTY(EditAnywhere, Category = "Layer", Meta = (EditCondition = "IsEnabled"))
	FColor Color {FColor::Blue};
	
	/** the procedural sound data of this layer. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sounds", Meta = (TitleProperty = "Name", EditCondition = "IsEnabled"))
	TArray<FAmbiverseProceduralSoundData> ProceduralSounds;

	/** Parameters that influence all procedural sounds in this layer. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parameters", Meta = (TitleProperty = "Name", EditCondition = "IsEnabled"))
	TArray<FAmbiverseParameterModifiers> Parameters;

	/** Volume multiplier for all sounds in this layer. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (DisplayName = "Volume", ClampMin = "0", ClampMax = "2", UIMin = "1", UIMax ="2"))
	float LayerVolume {1.0f};

	/** Rate multiplier for all sounds in this layer. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (DisplayName = "Density", ClampMin = "0", ClampMax = "2", UIMin = "1", UIMax ="2"))
	float LayerDensity {1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (DisplayName = "Enable Layer"))
	bool IsEnabled {true};
	
	UPROPERTY()
	TArray<FAmbiverseLayerQueueEntry> SoundQueue;

	UPROPERTY()
	FTimerHandle TimerHandle;
	
	FTimerDelegate TimerDelegate;

public:
	bool GetEntryWithLowestTime(FAmbiverseLayerQueueEntry& OutEntry);

	void SubtractTimeFromQueue(const float TimeToSubtract);
	
	void SortQueueDataByTime();
};


