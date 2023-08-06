// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AmbiverseElement.h"

class UAmbiverseDistributorAsset;
struct FAmbiverseElement;

class AmbiverseStatics
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
	
	/**
	 * Performs a random distribution around the listener.
	 *
	 * @param[out] OutTransform The transform of the element as a result of the computation.
	 * @param[in] ListenerTransform The transform of the listener.
	 * @param[in] Element The instance of the element to be distributed.
	 * @return true if the operation succeeded, false otherwise.
	 */
	static bool PerformRandomDistribution(FTransform& OutTransform, const FTransform& ListenerTransform,
		const FAmbiverseElement& Element);

	/**
	 * Performs a uniform distribution.
	 *
	 * @param[out] OutTransform The transform of the element as a result of the computation.
	 * @param[in] ListenerTransform The transform of the listener.
	 * @param[in] Element The instance of the element to be distributed.
	 * @param[in] IgnoreZ Whether to ignore the Z component of the vectors.
	 * @param[in] WorldContextObject The UObject to retrieve the UWorld ptr from.
	 * @return true if the operation succeeded, false otherwise.
	 */
	static bool PerformUniformDistribution(FTransform& OutTransform, const FTransform& ListenerTransform,
		const FAmbiverseElement& Element, const bool IgnoreZ, const UObject* WorldContextObject);

	
	/**
	 * Performs a static distribution.
	 *
	 * @param[out] OutTransform The transform of the element as a result of the computation.
	 * @param[in] ListenerTransform The transform of the listener.
	 * @param[in] Element The instance of the element to be distributed.
	 * @return true if the operation succeeded, false otherwise.
	 */
	static bool PerformStaticDistribution(FTransform& OutTransform, const FTransform& ListenerTransform,
		const FAmbiverseElement& Element);

	static bool PerformDistributorDistribution(FTransform& OutTransform, const FTransform& ListenerTransform,
		TSubclassOf<UAmbiverseDistributorAsset> Distributor);

	/**
     * Attempts to retrieve the transform of the primary
     * audio listener from the main audio device.
     * 
     * @param[out] Transform Reference to a FTransform that will hold the listener's transform 
     * if the function succeeds.
     * @return bool Returns true if the transform was successfully retrieved and false otherwise.
     * If false, the contents of the Transform parameter are undefined.
     */
	static bool GetListenerTransform(FTransform& Transform);

	static UAmbiverseSubsystem* GetSubsystem(const UObject* WorldContextObject);
	static UAmbiverseSubsystem* GetSubsystem(const UWorld* World);
};
