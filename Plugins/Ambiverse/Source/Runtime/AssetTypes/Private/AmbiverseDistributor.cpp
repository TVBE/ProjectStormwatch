// Copyright Notice

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
