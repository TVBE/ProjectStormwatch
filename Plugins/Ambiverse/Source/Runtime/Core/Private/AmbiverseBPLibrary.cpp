// Copyright (c) 2022-present Tim Verberne
// This source code is part of the Adaptive Ambience System plugin

#include "AmbiverseBPLibrary.h"
#include "Ambiverse.h"
#include "AmbiverseLayerManager.h"
#include "AmbiverseSubsystem.h"

UAmbiverseBPLibrary::UAmbiverseBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

void UAmbiverseBPLibrary::AddAmbiverseLayer(UObject* WorldContextObject, UAmbiverseLayer* AmbiverseLayer)
{
	if (!WorldContextObject) { return; }

	if (UWorld* World {WorldContextObject->GetWorld()})
	{
		if (UAmbiverseSubsystem* AmbiverseSubsystem { World->GetSubsystem<UAmbiverseSubsystem>()})
		{
			if (UAmbiverseLayerManager* LayerManager {AmbiverseSubsystem->GetLayerManager()})
			{
				LayerManager->AddAmbienceLayer(AmbiverseLayer);
			}
		}
	}
}

void UAmbiverseBPLibrary::PopAmbiverseLayer(UObject* WorldContextObject, UAmbiverseLayer* AmbiverseLayer)
{
	if (!WorldContextObject) { return; }

	if (UWorld* World {WorldContextObject->GetWorld()})
	{
		if (UAmbiverseSubsystem* AmbiverseSubsystem { World->GetSubsystem<UAmbiverseSubsystem>()})
		{
			if (UAmbiverseLayerManager* LayerManager {AmbiverseSubsystem->GetLayerManager()})
			{
				LayerManager->PopAmbienceLayer(AmbiverseLayer);
			}
		}
	}
}

void UAmbiverseBPLibrary::SetAmbiverseParameter(UObject* WorldContextObject)
{
	if (!WorldContextObject) { return; }

	if (UWorld* World {WorldContextObject->GetWorld()})
	{
		if (UAmbiverseSubsystem* AmbiverseSubsystem { World->GetSubsystem<UAmbiverseSubsystem>()})
		{
			
		}
	}
}
