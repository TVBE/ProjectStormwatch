// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#include "AmbiverseDistributorAsset.h"

DEFINE_LOG_CATEGORY_CLASS(UAmbiverseDistributorAsset, LogAmbiverseDistributor);

void UAmbiverseDistributorAsset::Initialize(const UObject* WorldContextObject)
{
	if (WorldContextObject)
	{
		World = WorldContextObject->GetWorld();
		
		if (APlayerController* PlayerController {WorldContextObject->GetWorld()->GetFirstPlayerController()})
		{
			if (APlayerCameraManager* CameraManager {PlayerController->PlayerCameraManager})
			{
				Listener = CameraManager->GetViewTarget();
			}
		}
	}
}

UWorld* UAmbiverseDistributorAsset::GetWorldFromListener()
{
	if (!Listener)
	{
		UE_LOG(LogAmbiverseDistributor, Warning, TEXT("GetWorldFromListener: Listener is null."));
		return nullptr;
	}

	return {Listener->GetWorld()};
}
