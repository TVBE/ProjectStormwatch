// Copyright (c) 2023-present Tim Verberne
// This source code is part of the Ambiverse plugin

#pragma once

#include "CoreMinimal.h"
#include "AmbiverseSubsystemComponent.h"
#include "AmbiverseDistributorManager.generated.h"

class UAmbiverseSubsystem;

/** Manages distributor instances. */
UCLASS()
class UAmbiverseDistributorManager : public UAmbiverseSubsystemComponent
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogAmbiverseDistributorManager, Log, All)

	UPROPERTY(Transient)
	UAmbiverseSubsystem* AmbiverseSubsystem {nullptr};
	
};
