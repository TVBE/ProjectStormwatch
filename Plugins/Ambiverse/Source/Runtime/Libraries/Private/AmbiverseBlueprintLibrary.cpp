// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#include "AmbiverseBlueprintLibrary.h"

bool UAmbiverseBlueprintLibrary::StartAmbiverseScene(const UObject* WorldContextObject, UAmbiverseSceneAsset* Asset)
{
	if (UAmbiverseSubsystem* AmbiverseSubsystem = WorldContextObject->GetWorld()->GetSubsystem<UAmbiverseSubsystem>())
	{
		// Assuming SceneManager is publicly accessible or there's a getter method for it
		UAmbiverseLayerManager* LayerManager = AmbiverseSubsystem->GetLayerManager();
		if (LayerManager)
		{
			return LayerManager->RegisterAmbiverseLayer(Asset);
		}
	}
	return false;
}

bool UAmbiverseBlueprintLibrary::StopAmbiverseScene(const UObject* WorldContextObject, UAmbiverseSceneAsset* Asset,
	const bool ForceStop, const float FadeTime)
{
	if (UAmbiverseSubsystem* AmbiverseSubsystem = WorldContextObject->GetWorld()->GetSubsystem<UAmbiverseSubsystem>())
	{
		// Assuming LayerManager is publicly accessible or there's a getter method for it
		UAmbiverseLayerManager* LayerManager = AmbiverseSubsystem->GetLayerManager();
		if (LayerManager)
		{
			return LayerManager->UnregisterAmbiverseLayer(Asset, ForceStop, FadeTime);
		}
	}
	return false;
}
