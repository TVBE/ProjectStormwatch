// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuditoryEvent.generated.h"

/** An Auditory Event is used to increase the heatmap of a room. This can be used to notify the nightstalker of sounds in a room. */
USTRUCT(BlueprintType, Category = "Sensory Events",
	Meta = (DisplayName = "Auditory Event", ShortToolTip = "Event that is used to notify the NightStalker of a sound. "))
struct FAuditoryEvent
{
	GENERATED_BODY()
	
	/** The loudness of the event, measured in decibels. */
	UPROPERTY(BlueprintReadWrite, Category = "AuditoryEvent",
		Meta = (DisplayName = "Loudness", ClampMin = "30", ClampMax = "120", UIMin = "30", UIMax = "120", Delta = "0.1", ForceUnits = "dB"))
	float Loudness {30.0f};

	/** Value that scales how much this sound attracts the Nightstalker.
	 *	Higher values cause this event to increase the heatmap of the room by a larger amount. */
	UPROPERTY(BlueprintReadWrite, Category = "AuditoryEvent",
		Meta = (DisplayName = "Attraction", ClampMin = "0", ClampMax = "4", UIMin = "0", UIMax = "4", Delta = "0.1"))
	float AttractionFactor {1.0f};

	/** Value that scales how much this sound propagates through the area. */
	UPROPERTY(BlueprintReadWrite, Category = "AuditoryEvent",
		Meta = (DisplayName = "Propagation", ClampMin = "0", ClampMax = "2", UIMin = "0", UIMax = "2", Delta = "0.1"))
	float PropagationFactor {1.0f};

	
};
