// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#include "AmbiverseStatics.h"

#include "AudioDevice.h"

bool AmbiverseStatics::GetListenerTransform(FTransform& Transform)
{
	if (const FAudioDevice* AudioDevice {GEngine->GetMainAudioDevice()}; AudioDevice->GetListeners().Num() > 0)
	{
		Transform = AudioDevice->GetListeners()[0].Transform;
		return true;
	}
	return false;
}
