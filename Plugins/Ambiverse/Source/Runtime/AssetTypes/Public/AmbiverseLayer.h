// Copyright (c) 2023-present Tim Verberne
// This source code is part of the Ambiverse plugin

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
	FAmbiverseProceduralElementData SoundData {FAmbiverseProceduralElementData()};

	UPROPERTY()
	float Time {0.0f};
	
	/** The reference time the entry was initialized with.
	 *	We use this time value to be able to dynamically apply parameters in real time without breaking th existing queue. */
	UPROPERTY()
	float ReferenceTime {0.0f};
};

UCLASS(Blueprintable, BlueprintType, ClassGroup = "Ambiverse", Meta = (DisplayName = "Ambiverse Layer",
	ShortToolTip = "A list of Ambiverse Elements that can be procedurally played."))
class AMBIVERSE_API UAmbiverseLayer : public UObject
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogAmbienceLayer, Log, All)

public:
#if WITH_EDITORONLY_DATA
	/** User friendly name for the layer. */
	UPROPERTY(EditAnywhere, Category = "Editor")
	FName Name;

	/** A description for the layer. */
	UPROPERTY(EditAnywhere, Category = "Editor", Meta = (MultiLine = "true"))
	FText Description;
	
	/** The color of the layer when visualised in the editor. */
	UPROPERTY(EditAnywhere, Category = "Editor")
	FColor Color {FColor::Blue};
#endif
	
	/** the procedural sound data of this layer. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Elements", Meta = (TitleProperty = "Name"))
	TArray<FAmbiverseProceduralElementData> Elements;

	/** Parameters that influence all procedural sounds in this layer. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parameters")
	TArray<FAmbiverseParameterModifiers> Parameters;

	/** Volume multiplier for all sounds in this layer. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (DisplayName = "Volume", ClampMin = "0", ClampMax = "2", UIMin = "1", UIMax ="2"))
	float LayerVolume {1.0f};

	/** Rate multiplier for all sounds in this layer. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (DisplayName = "Density", ClampMin = "0", ClampMax = "2", UIMin = "1", UIMax ="2"))
	float LayerDensity {1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (DisplayName = "Enable Layer"))
	bool IsEnabled {true};

	
	/** Runtime */
	
	UPROPERTY()
	TArray<FAmbiverseLayerQueueEntry> SoundQueue;

	UPROPERTY()
	FTimerHandle TimerHandle;
	
	FTimerDelegate TimerDelegate;

public:
	bool GetEntryWithLowestTime(FAmbiverseLayerQueueEntry& OutEntry);

	void SubtractTimeFromQueue(const float TimeToSubtract);
	
	void SortQueueDataByTime();

	void InitializeSoundQueue();
};




