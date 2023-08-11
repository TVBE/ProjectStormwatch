// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AmbiverseSubsystem.h"
#include "AmbiverseStatics.generated.h"

UCLASS()
class AMBIVERSE_API UAmbiverseStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	/**
	* Starts (activates) an Ambiverse scene if it is not already active. 
	* @param WorldContextObject The context object for accessing world.
	* @param Asset The scene to start.
	* @return Returns true if the scene was not active and has been successfully started. 
	*/
	UFUNCTION(BlueprintCallable, Category = "Ambiverse", Meta = (DisplayName = "Start Scene", WorldContext = "WorldContextObject"))
	static bool StartAmbiverseScene(const UObject* WorldContextObject, UAmbiverseSceneAsset* Asset);
	
	/**
	* Stops (deactivates) an Ambiverse scene if it is currently active.
	* @param WorldContextObject The context object for accessing world.
	* @param Asset The scene to stop.
	* @param ForceStop If true, all elements from the scene that are currently playing are stopped.
	* @param FadeTime An optional fade time for all elements to make the transition smoother.
	* @return Whether the specified scene was found and stopped successfully. This will be true even if some of the scene's elements are still playing.
	*/
	UFUNCTION(BlueprintCallable, Category = "Ambiverse", Meta = (DisplayName = "Stop Scene", WorldContext = "WorldContextObject"))
	static bool StopAmbiverseScene(const UObject* WorldContextObject, UAmbiverseSceneAsset* Asset, const bool ForceStop = false, const float FadeTime = 1.0f);

	static UAmbiverseSubsystem* GetSubsystem(const UObject* WorldContextObject);
	static UAmbiverseSubsystem* GetSubsystem(const UWorld* World);
};



