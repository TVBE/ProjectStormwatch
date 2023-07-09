// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#pragma once

#include "AmbiverseElementAsset.h"
#include "AmbiverseElementRuntimeData.generated.h"

class UAmbiverseElementAsset;

UENUM(BlueprintType)
enum class EIntervalMode : uint8
{
	OnSpawn			UMETA(DisplayName = "Start on Spawn"),
	OnFinished		UMETA(DisplayName = "Start on End")
};

/** Defines the spawning behavior of a spatial AmbienceSoundSource */
USTRUCT(BlueprintType)
struct FAmbiverseElementRuntimeData
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, Meta = (DisplayName = "Element"))
	UAmbiverseElementAsset* ElementAsset {nullptr};

	/** The volume of this element. */
	UPROPERTY(EditAnywhere, Meta = (ClampMin = "0", ClampMax = "2", UIMin = "0", UIMax = "2"))
	float Volume {1.0f};
	
	/** The minimum and maximum delay between instances of an AmbienceSystem preset entry occuring.
	 *	The play interval is randomized between these two values. */
	UPROPERTY(EditAnywhere, Meta = (DisplayName = "Interval", ClampMin = "0", Delta = "1"))
	FVector2D IntervalRange {FVector2D(10, 30)};

	UPROPERTY(EditAnywhere, Meta = (DisplayName = "Interval Mode"))
	EIntervalMode IntervalMode {EIntervalMode::OnFinished};

	bool IsValid() const
	{
		return (ElementAsset != nullptr);
	}
};


