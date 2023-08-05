// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AmbiverseElementRuntimeData.h"
#include "AmbiverseLayer.generated.h"

struct FAmbiverseElement;

USTRUCT()
struct FSceneLifetimeData
{
	GENERATED_USTRUCT_BODY()
	
	bool HasLifetime {false};

	/** The total lifetime duration of the scene. */
	float Lifetime {0.0f};

	/** @brief The normalized ratio of the active duration of the scene relative to its maximum lifetime.
	* This property provides a normalized value indicating how much of the scene's total lifetime has been used up.
	* It is calculated as the active duration divided by the total lifetime. 
	* For instance, if the active duration is 20 seconds and the total lifetime is 40 seconds, LifetimeRatio will be 0.5. */
	float LifetimeRatio {0.0f};
	
	float TimeActive {0.0f};
	
	FSceneLifetimeData(float InLifetime)
	: Lifetime(InLifetime)
	, HasLifetime(true)
	{
	}

	void IncrementLifetime(float DeltaTime)
	{
		TimeActive += DeltaTime;
	
		if(HasLifetime)
		{
			LifetimeRatio = FMath::Clamp(TimeActive / Lifetime, 0.0f, 1.0f);
		}
	}
};

USTRUCT()
struct FAmbiverseScene
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY()
	UAmbiverseSceneAsset* Asset;

	UPROPERTY()
	TArray<FAmbiverseElement> Elements;

	UPROPERTY()
	FSceneLifetimeData LifetimeData;
	
	bool IsInitialized {false};
};
