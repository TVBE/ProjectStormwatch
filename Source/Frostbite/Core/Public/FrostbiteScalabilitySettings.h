// Copyright 2023 Barrelhouse

#pragma once

#include "CoreMinimal.h"
#include "FrostbiteScalabilitySettings.generated.h"

/** Struct defining the graphical settings for the game. */
USTRUCT(BlueprintType)
struct FFrostbiteScalabilitySettings
{
	GENERATED_USTRUCT_BODY()

	/***/
 
	/** Constructor with default values. */
	FFrostbiteScalabilitySettings()
	{
	}
 
};

/** Enum for defining which foot performed a footstep. */
UENUM(BlueprintType)
enum class EAntiAliasingSetting : uint8
{
	TSR			UMETA(DisplayName = "Temporal Super Resolution"),
	TAA			UMETA(DisplayName = "Temporal Anti-Aliasing"),
	FXAA		UMETA(DisplayName = "Fast Approximate Anti-Aliasing"),
};
