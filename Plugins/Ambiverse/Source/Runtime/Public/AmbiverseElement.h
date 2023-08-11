// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AmbiverseSceneAsset.h"
#include "AmbiverseElement.generated.h"

class UAmbiverseElementAsset;
class UAmbiverseSubsystem;
class AAmbiverseSoundSource;

UENUM(BlueprintType)
enum class EElementIntervalMode : uint8
{
	OnSpawn			UMETA(DisplayName = "Start on Spawn"),
	OnFinished		UMETA(DisplayName = "Start on End")
};

// TODO: Finish implementation.
UENUM(BlueprintType)
enum class EElementVerticalReference : uint8
{
	RelativeToListener				UMETA(DisplayName = "Relative To Listener", ToolTip = "We will calculate vertical offset in relation to the listener's position."),
	RelativeToGeometry				UMETA(DisplayName = "Relative To Geometry", ToolTip = "We will calculate vertical offset in relation to the geometry at the XY point of placement."),
	RelativeToTerrain				UMETA(DisplayName = "Relative To Terrain", ToolTip = "We will calculate vertical offset in relation to the terrain at the XY point of placement."),
	Absolute						UMETA(DisplayName = "Absolute", ToolTip = "We will convert our vertical offset to an absolute world position. "),
};

// TODO: Finish implementation.
UENUM(BlueprintType)
enum class EElementDistributionMode : uint8
{
	Random							UMETA(DisplayName = "Random", ToolTip = "We distribute the element randomly."),
	Uniform							UMETA(DisplayName = "Uniform", ToolTip = "We distribute the element pseudo-evenly."),
	Static							UMETA(DisplayName = "Static", ToolTip = "We try to place the element around the same location until a threshold has been met. This creates more consistent soundscapes."),
};

/** Defines the area around the player that an AmbienceSoundSource can play in. */
USTRUCT(BlueprintType)
struct FElementDistributionData
{
	GENERATED_USTRUCT_BODY()

	/** Defines the placement mode for the element. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Distribution")
	EElementDistributionMode DistributionMode {EElementDistributionMode::Random};

	/** The distance threshold between the listener and the element's static location before we choose another location for our element. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Distribution", Meta = (EditCondition = "DistributionMode == EDistributionMode::Static", EditConditionHides))
	float Threshold {5000.0f};

	/** The amount of deviation allowed each time the static element is triggered. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Distribution", Meta = (EditCondition = "DistributionMode == EDistributionMode::Static || DistributionMode == EDistributionMode::Uniform", EditConditionHides))
	float Drift {0.0f};
	
	/** Defines the horizontal range in which a sound source can play in relation to the player. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Horizontal Distribution", Meta = (ClampMin = "0"))
	FVector2D HorizontalRange {FVector2D(2000, 3000)};

	/** Defines the vertical distribution perspective. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vertical Distribution")
	EElementVerticalReference VerticalDistributionMode {EElementVerticalReference::RelativeToListener};

	/** Defines a vertical offset. This can be used to ensure that the sound is always played above or below the player. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vertical Distribution")
	float VerticalOffset {0.0f};
	
	/** Defines the vertical range in which a sound source can play in relation to the player. Is not affected by ExclusionRadius. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vertical Distribution")
	float VerticalRange {100.0f};
};

USTRUCT(BlueprintType)
struct FElementIntervalData
{
	GENERATED_BODY()
    
	/** The minimum and maximum delay between instances of an AmbienceSystem preset entry occuring.
	 *	The play interval is randomized between these two values. */
	UPROPERTY(EditAnywhere, Meta = (DisplayName = "Range", ClampMin = "0", Delta = "1"))
	FVector2D Range { FVector2D(10, 30) };
    
	UPROPERTY(EditAnywhere, Meta = (DisplayName = "Mode"))
	EElementIntervalMode Mode { EElementIntervalMode::OnFinished };
};

USTRUCT()
struct FElementTimeData
{
	GENERATED_USTRUCT_BODY()

	float Time {0.0f};

	/** The reference time the entry was initialized with.
	 *	We use this time value to be able to dynamically apply parameters in real time without breaking the existing schedule. */
	float ReferenceTime {0.0f};
};

USTRUCT()
struct FAmbiverseElement
{
	GENERATED_USTRUCT_BODY()
	
	TStrongObjectPtr<const UAmbiverseElementAsset> Asset;
	
	const FElementDistributionData DistributionData;
	const FElementIntervalData IntervalData;
	FElementTimeData TimeData;
	
	/** The last location this element was played at. We only really use this if the distribution mode is set to static.
	 *	We save this vector here independently from the SoundSource used to play this element,
	 *	as the SoundSource might have been re-used for another element before. */ 
	TOptional<FTransform> LastPlayTransform;

	bool IsPlaying {false};
	bool IsPendingKill {false};
	
	FAmbiverseElement(
	const UAmbiverseElementAsset*	RawAssetPtr,
	const FElementDistributionData& InDistributionData,
	const FElementIntervalData&		InIntervalData,
	const FElementTimeData&			InTimeData
)
	: Asset(RawAssetPtr)
	, DistributionData(InDistributionData)
	, IntervalData(InIntervalData)
	, TimeData(InTimeData)
	{
	}
	
	bool IsValid() const { return Asset.IsValid(); }
	
	const UAmbiverseElementAsset* GetAsset() const { return Asset.Get(); }
};
