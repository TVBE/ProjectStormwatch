// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#pragma once

#include "AmbiverseSoundDistributionData.generated.h"

// TODO: Finish implementation.
UENUM(BlueprintType)
enum class EVerticalDistributionMode : uint8
{
	RelativeToListener				UMETA(DisplayName = "Relative To Listener", ToolTip = "We will calculate vertical offset in relation to the listener's position."),
	RelativeToGeometry				UMETA(DisplayName = "Relative To Geometry", ToolTip = "We will calculate vertical offset in relation to the geometry at the XY point of placement."),
	RelativeToTerrain				UMETA(DisplayName = "Relative To Terrain", ToolTip = "We will calculate vertical offset in relation to the terrain at the XY point of placement."),
	Absolute						UMETA(DisplayName = "Absolute", ToolTip = "We will convert our vertical offset to an absolute world position. "),
};

// TODO: Finish implementation.
UENUM(BlueprintType)
enum class EDistributionMode : uint8
{
	Random							UMETA(DisplayName = "Random", ToolTip = "We distribute the element randomly."),
	Uniform							UMETA(DisplayName = "Uniform", ToolTip = "We distribute the element pseudo-evenly."),
	Static							UMETA(DisplayName = "Static", ToolTip = "We try to place the element around the same location until a threshold has been met. This creates more consistent soundscapes."),
};

/** Defines the area around the player that an AmbienceSoundSource can play in. */
USTRUCT(BlueprintType)
struct FAmbiverseSoundDistributionData
{
	GENERATED_USTRUCT_BODY()

	/** Defines the placement mode for the element. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Distribution")
	EDistributionMode DistributionMode {EDistributionMode::Random};

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
	EVerticalDistributionMode VerticalDistributionMode {EVerticalDistributionMode::RelativeToListener};

	/** Defines a vertical offset. This can be used to ensure that the sound is always played above or below the player. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vertical Distribution")
	float VerticalOffset {0.0f};
	
	/** Defines the vertical range in which a sound source can play in relation to the player. Is not affected by ExclusionRadius. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vertical Distribution")
	float VerticalRange {100.0f};
};
