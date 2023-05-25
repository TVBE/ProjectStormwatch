// Copyright (c) 2023-present Tim Verberne
// This source code is part of the Ambiverse plugin

#include "AmbiverseDistributorManager.h"

#include "AmbiverseDistributor.h"

DEFINE_LOG_CATEGORY_CLASS(UAmbiverseDistributorManager, LogAmbiverseDistributorManager);

UAmbiverseDistributor* UAmbiverseDistributorManager::GetDistributorByClass(TSubclassOf<UAmbiverseDistributor> Class)
{
	if (!Owner) { return nullptr ; }
	
	for (UAmbiverseDistributor* Distributor : DistributorRegistry)
	{
		if (Distributor->StaticClass() == Class)
		{
			UE_LOG(LogAmbiverseDistributorManager, VeryVerbose, TEXT("GetDistributorByClass: Found existing distributor of class: %s"), *Class->GetName());
			return Distributor;
		}
	}

	/** If no instance of the specified distributor class was found, we instance a new one and return it. */
	UAmbiverseDistributor* Distributor = NewObject<UAmbiverseDistributor>(this, Class.Get());
	UE_LOG(LogAmbiverseDistributorManager, Verbose, TEXT("GetDistributorByClass: Created new distributor of class: %s"), *Class->GetName());
	
	return Distributor;
}
