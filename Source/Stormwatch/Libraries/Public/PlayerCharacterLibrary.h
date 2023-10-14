// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BHPlayerCharacterLibrary.generated.h"

/**
 * 
 */
UCLASS()
class STORMWATCH_API UPlayerCharacterLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	/** Returns data about a footstep for the specified actor, like the object or physical material underneath the foot at the time of the footstep.
	*	@Return FootstepData structure containing relevant information about the location and velocity of the foot at the time of the footstep. 
	*/
	UFUNCTION(BlueprintPure, Category = "Character", Meta = (DisplayName = "Get Footstep Data Underneath Actor", WorldContext = "WorldContextObject"))
	static FBHStepData GetStepData(const UObject* WorldContextObject, const AActor* Actor, const float TraceLength);
};
