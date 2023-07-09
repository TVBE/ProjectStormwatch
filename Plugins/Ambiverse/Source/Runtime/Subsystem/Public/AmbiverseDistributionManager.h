// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AmbiverseSubsystemComponent.h"
#include "AmbiverseDistributionManager.generated.h"

class UAmbiverseDistributorAsset;
class UAmbiverseSubsystem;
class UAmbiverseElementInstance;

/** Manages sound source distribution. */
UCLASS()
class UAmbiverseDistributionManager : public UAmbiverseSubsystemComponent
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogAmbiverseDistributionManager, Log, All)

private:
	UPROPERTY()
	TArray<UAmbiverseDistributorAsset*> Distributors;

public:
	bool GetTransformForElement(FTransform& Transform, UAmbiverseElementInstance* ElementInstance);
	
	/** Searches for a distributor instance in the registry. Will instance one if no instance was found. */
	UAmbiverseDistributorAsset* GetDistributorByClass(TSubclassOf<UAmbiverseDistributorAsset> Class);

private:
	/**
	 * Performs a random distribution around the listener.
	 *
	 * @param[out] OutTransform The transform of the element as a result of the computation.
	 * @param[in] ListenerTransform The transform of the listener.
	 * @param[in] ElementInstance The instance of the element to be distributed.
	 * @return true if the operation succeeded, false otherwise.
	 */
	static bool PerformRandomDistribution(FTransform& OutTransform, const FTransform& ListenerTransform,
		UAmbiverseElementInstance* ElementInstance);

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
		UAmbiverseElementInstance* ElementInstance, const TArray<FVector>& Vectors, const bool IgnoreZ);

	
	/**
	 * Performs a static distribution.
	 *
	 * @param[out] OutTransform The transform of the element as a result of the computation.
	 * @param[in] ListenerTransform The transform of the listener.
	 * @param[in] ElementInstance The instance of the element to be distributed.
	 * @return true if the operation succeeded, false otherwise.
	 */
	static bool PerformStaticDistribution(FTransform& OutTransform, const FTransform& ListenerTransform,
		UAmbiverseElementInstance* ElementInstance);

public:
	FORCEINLINE TArray<UAmbiverseDistributorAsset*> GetDistributors() const { return Distributors; }
};


