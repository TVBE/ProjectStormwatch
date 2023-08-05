// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#pragma once

#include "CoreMinimal.h"

class UAmbiverseDistributorAsset;
struct FAmbiverseElement;

class AmbiverseStatics
{
public:
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

	/**
	 * Performs a random distribution around the listener.
	 *
	 * @param[out] OutTransform The transform of the element as a result of the computation.
	 * @param[in] ListenerTransform The transform of the listener.
	 * @param[in] ElementInstance The instance of the element to be distributed.
	 * @return true if the operation succeeded, false otherwise.
	 */
	static bool PerformRandomDistribution(FTransform& OutTransform, const FTransform& ListenerTransform,
		FAmbiverseElement& ElementInstance);

	/**
	 * Performs a uniform distribution.
	 *
	 * @param[out] OutTransform The transform of the element as a result of the computation.
	 * @param[in] ListenerTransform The transform of the listener.
	 * @param[in] ElementInstance The instance of the element to be distributed.
	 * @param[in] Vectors The vectors used for the computation.
	 * @param[in] IgnoreZ Whether to ignore the Z component of the vectors.
	 * @return true if the operation succeeded, false otherwise.
	 */
	static bool PerformUniformDistribution(FTransform& OutTransform, const FTransform& ListenerTransform,
		FAmbiverseElement& ElementInstance, const TArray<FVector>& Vectors, const bool IgnoreZ);

	
	/**
	 * Performs a static distribution.
	 *
	 * @param[out] OutTransform The transform of the element as a result of the computation.
	 * @param[in] ListenerTransform The transform of the listener.
	 * @param[in] ElementInstance The instance of the element to be distributed.
	 * @return true if the operation succeeded, false otherwise.
	 */
	static bool PerformStaticDistribution(FTransform& OutTransform, const FTransform& ListenerTransform,
		FAmbiverseElement& ElementInstance);

	static bool PerformDistributorDistribution(FTransform& OutTransform, const FTransform& ListenerTransform,
		TSubclassOf<UAmbiverseDistributorAsset> Distributor);
};
