// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AmbiverseElementRuntimeData.h"
#include "AmbiverseLayer.generated.h"

class UAmbiverseSubsystem;

UCLASS(MinimalAPI, Transient)
class UAmbiverseLayer : public UObject
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogAmbiverseRuntimeLayer, Log, All)

public:
	UPROPERTY()
	UAmbiverseLayerAsset* Asset;
	
	/** @brief The normalized ratio of the active duration of the layer relative to its maximum lifetime.
	* This property provides a normalized value indicating how much of the layer's total lifetime has been used up.
	* It is calculated as the active duration divided by the total lifetime. 
	* For instance, if the active duration is 20 seconds and the total lifetime is 40 seconds, LifetimePercentage will be 0.5. */
	float LifetimeRatio {0.0f};
	
	UPROPERTY(Transient)
	TArray<UAmbiverseElement*> ProceduralElements;

private:
	UPROPERTY(Transient)
	UAmbiverseSubsystem* AmbiverseSubsystem;
	
	bool IsInitialized {false};
	
	float TimeActive {0.0f};

public:
	bool Initialize(UAmbiverseSubsystem* Subsystem);
	
	static UAmbiverseLayer* CreateInstanceFromAsset(UObject* Outer, UAmbiverseLayerAsset* Asset);
};
