// Copyright (c) 2023-present Tim Verberne
// This source code is part of the Ambiverse plugin

#pragma once

#include "CoreMinimal.h"
#include "AmbiverseSubsystemComponent.h"
#include "AmbiverseDistributorManager.generated.h"

class UAmbiverseDistributor;
class UAmbiverseSubsystem;

/** Manages distributor instances. */
UCLASS()
class UAmbiverseDistributorManager : public UAmbiverseSubsystemComponent
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogAmbiverseDistributorManager, Log, All)

private:
	UPROPERTY()
	TArray<UAmbiverseDistributor*> DistributorRegistry;

public:
	/** Searches for a distributor instance in the registry. Will instance one if no instance was found. */
	UAmbiverseDistributor* GetDistributorByClass(TSubclassOf<UAmbiverseDistributor> Class);
	
	FORCEINLINE TArray<UAmbiverseDistributor*> GetDistributorRegistry() const { return DistributorRegistry; }
};
