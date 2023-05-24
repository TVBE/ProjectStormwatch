// Copyright (c) 2023-present Tim Verberne
// This source code is part of the Ambiverse plugin

#include "AmbiverseElement.h"

DEFINE_LOG_CATEGORY_CLASS(UAmbiverseElement, LogAmbiverseElement);

UMetaSoundSource* UAmbiverseElement::GetSoundFromMap(const TMap<UMetaSoundSource*, int>& SoundMap)
{
	return nullptr;
}

#if WITH_EDITOR
void UAmbiverseElement::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UAmbiverseElement, Sounds))
	{
		for (auto& Entry : Sounds)
		{
			if (Entry.Value < 1)
			{
				Entry.Value = 1;
			}
		}
	}
}
#endif

