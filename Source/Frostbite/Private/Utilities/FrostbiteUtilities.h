// Copyright 2023 Barrelhouse

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FrostbiteUtilities.generated.h"

/**
 * 
 */
UCLASS()
class UFrostbiteUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	/** Returns data about a footstep for the specified actor, like the object or physical material underneath the foot at the time of the footstep.
	*	@Return FootstepData structure containing relevant information about the location and velocity of the foot at the time of the footstep. 
	*/
	UFUNCTION(BlueprintPure, Category = "FrostbiteUtilities|Character", Meta = (DisplayName = "Get Footstep Data Underneath Actor"))
	static FFootstepData GetFootstepData(const UWorld* World, const AActor* Actor, const float TraceLength);
};
