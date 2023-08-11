// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#include "..\Public\AmbiverseStatics.h"

bool UAmbiverseStatics::StartAmbiverseScene(const UObject* WorldContextObject, UAmbiverseSceneAsset* Asset)
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

bool UAmbiverseStatics::StopAmbiverseScene(const UObject* WorldContextObject, UAmbiverseSceneAsset* Asset,
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

UAmbiverseSubsystem* UAmbiverseStatics::GetSubsystem(const UObject* WorldContextObject)
{
	if (!WorldContextObject || !WorldContextObject->ImplementsGetWorld()) { return nullptr; }
	return GetSubsystem(WorldContextObject->GetWorld());
}

UAmbiverseSubsystem* UAmbiverseStatics::GetSubsystem(const UWorld* World)
{
	if (!World) { return nullptr; }
	return World->GetSubsystem<UAmbiverseSubsystem>();
}
