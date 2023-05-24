// Copyright (c) 2023-present Tim Verberne
// This source code is part of the Ambiverse plugin

#include "AmbiverseDistributor.h"

void UAmbiverseDistributor::Activate(UObject* WorldContextObject)
{
	if (WorldContextObject)
	{
		if (APlayerController* PlayerController {WorldContextObject->GetWorld()->GetFirstPlayerController()})
		{
			if (APlayerCameraManager* CameraManager {PlayerController->PlayerCameraManager})
			{
				Listener = CameraManager->GetViewTarget();
			}
		}
	}
}
