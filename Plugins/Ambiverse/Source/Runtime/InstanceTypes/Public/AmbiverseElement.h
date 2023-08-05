// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AmbiverseSceneAsset.h"
#include "AmbiverseElementRuntimeData.h"
#include "AmbiverseElement.generated.h"

class UAmbiverseSubsystem;
class AAmbiverseSoundSource;

USTRUCT()
struct FElementTimeData
{
	GENERATED_USTRUCT_BODY()

	float Time {0.0f};

	/** The reference time the entry was initialized with.
	 *	We use this time value to be able to dynamically apply parameters in real time without breaking the existing schedule. */
	float ReferenceTime {0.0f};
};

UENUM(BlueprintType)
enum class EIntervalMode : uint8
{
	OnSpawn			UMETA(DisplayName = "Start on Spawn"),
	OnFinished		UMETA(DisplayName = "Start on End")
};

USTRUCT()
struct FAmbiverseElement
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, Meta = (DisplayName = "Element"))
	UAmbiverseElementAsset* Asset {nullptr};

	/** The volume of this element. */
	UPROPERTY(EditAnywhere, Meta = (ClampMin = "0", ClampMax = "2", UIMin = "0", UIMax = "2"))
	float Volume {1.0f};
	
	/** The minimum and maximum delay between instances of an AmbienceSystem preset entry occuring.
	 *	The play interval is randomized between these two values. */
	UPROPERTY(EditAnywhere, Meta = (DisplayName = "Interval", ClampMin = "0", Delta = "1"))
	FVector2D IntervalRange {FVector2D(10, 30)};

	UPROPERTY(EditAnywhere, Meta = (DisplayName = "Interval Mode"))
	EIntervalMode IntervalMode {EIntervalMode::OnFinished};
	
	UPROPERTY(Transient)
	FElementTimeData TimeData;
	
	/** The Ambiverse layer this element is associated with. This can be a nullptr if the element was instanced without a layer. this is rare though. */
	UPROPERTY(Transient)
	UAmbiverseSceneAsset* AssociatedScene {nullptr};

	/** The last location this element was played at. We only really use this if the distribution mode is set to static.
	 *	We save this vector here independently from the SoundSource used to play this element, as the SoundSource might have been re-used for another element before
	 *	we need this data. */
	UPROPERTY(Transient)
	FVector LastPlaylocation;

	bool IsPlaying {false};
	bool IsPendingKill {false};
	
	FAmbiverseElement(UAmbiverseSceneAsset* Layer)
		: AssociatedScene(Layer)
	{
	}

	bool IsValid() const { return Asset != nullptr; }
};
