// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AmbiverseElement.h"

class UAmbiverseDistributor;
struct FAmbiverseElement;

struct DistributionParamameters
{
	FTransform& OutTransform;
	const FTransform& ListenerTransform;
	const FAmbiverseElement& Element;
	const UObject* WorldContextObject;

	DistributionParamameters(
	FTransform&					InOutTransform,
	const FTransform&			InListenerTransform,
	const FAmbiverseElement&	InElement,
	const UObject*				InWorldContextObject
	)
	: OutTransform(InOutTransform) 
	, ListenerTransform(InListenerTransform) 
	, Element(InElement) 
	, WorldContextObject(InWorldContextObject)
	{}
};

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
	static bool PerformRandomDistribution(const DistributionParamameters& Params);
	
	static bool PerformUniformDistribution(const DistributionParamameters& Params);
	
	static bool PerformStaticDistribution(const DistributionParamameters& Params);

	static bool PerformDistributorDistribution(const DistributionParamameters& Params,
		const TSubclassOf<UAmbiverseDistributor> Distributor);
	
	static bool GetListenerTransform(FTransform& Transform);
};
