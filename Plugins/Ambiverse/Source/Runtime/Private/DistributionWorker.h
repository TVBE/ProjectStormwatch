// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AmbiverseElement.h"

class UAmbiverseDistributorAsset;
struct FAmbiverseElement;

class DistributionWorker
{
public:
	/**
	 * Calculates the transform for an element.
	 *
	 * @param[out] OutTransform The transform of the element as a result of the computation.
	 * @param[in] Element The element to perform the calculation for.
	 * @param[in] WorldContextObject The UObject to retrieve the UWorld ptr from.
	 * @return true if the operation succeeded, false otherwise.
	 */
	static bool GetTransformForElement(FTransform& OutTransform, const FAmbiverseElement& Element,
		const UObject* WorldContextObject);

private:
	static bool PerformRandomDistribution(FTransform& OutTransform, const FTransform& ListenerTransform,
		const FAmbiverseElement& Element);
	
	static bool PerformUniformDistribution(FTransform& OutTransform, const FTransform& ListenerTransform,
		const FAmbiverseElement& Element, const bool IgnoreZ, const UObject* WorldContextObject);
	
	static bool PerformStaticDistribution(FTransform& OutTransform, const FTransform& ListenerTransform,
		const FAmbiverseElement& Element);

	static bool PerformDistributorDistribution(FTransform& OutTransform, const FTransform& ListenerTransform,
		TSubclassOf<UAmbiverseDistributorAsset> Distributor);
	
	static bool GetListenerTransform(FTransform& Transform);

	static UAmbiverseSubsystem* GetSubsystem(const UObject* WorldContextObject);
	static UAmbiverseSubsystem* GetSubsystem(const UWorld* World);
};
