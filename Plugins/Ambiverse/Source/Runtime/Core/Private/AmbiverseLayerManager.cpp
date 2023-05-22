// Copyright Notice

#include "AmbiverseLayerManager.h"

#include "AmbiverseLayer.h"
#include "AmbiverseProceduralSoundData.h"

DEFINE_LOG_CATEGORY_CLASS(UAmbiverseLayerManager, LogAmbiverseLayerManager);

void UAmbiverseLayerManager::AddAmbienceLayer(UAmbiverseLayer* Layer)
{
	if (!Layer)
	{
		UE_LOG(LogAmbiverseLayerManager, Warning, TEXT("AddAmbienceLayer: No Layer provided."));
		return;
	}

	if (Layer->ProceduralSounds.IsEmpty())
	{
		UE_LOG(LogAmbiverseLayerManager, Warning, TEXT("AddAmbienceLayer: Layer has no procedural sounds: '%s'."), *Layer->GetName());
		return;
	}

	for (FAmbiverseProceduralSoundData& SoundData : Layer->ProceduralSounds)
	{
		FAmbiverseProceduralSoundData::Validate(SoundData);
	}

	bool HasValidSoundData = false;
	for (const FAmbiverseProceduralSoundData& SoundData : Layer->ProceduralSounds)
	{
		if (SoundData.IsValid)
		{
			HasValidSoundData = true;
			break;
		}
	}

	if (!HasValidSoundData)
	{
		UE_LOG(LogAmbiverseLayerManager, Warning, TEXT("AddAmbienceLayer: Layer has no valid procedural sounds: '%s'."), *Layer->GetName());
		return;
	}

	
	if (!FindActiveAmbienceLayer(Layer))
	{
		LayerRegistry.Add(Layer);
		Layer->InitializeSoundQueue();
		
		UE_LOG(LogAmbiverseLayerManager, Verbose, TEXT("AddAmbienceLayer: Layer added successfully: '%s'."), *Layer->GetName());
	}
}

void UAmbiverseLayerManager::PopAmbienceLayer(UAmbiverseLayer* Layer)
{
	if (!Layer) { return; }
}

UAmbiverseLayer* UAmbiverseLayerManager::FindActiveAmbienceLayer(const UAmbiverseLayer* LayerToFind) const
{
	for (UAmbiverseLayer* Layer : LayerRegistry)
	{
		if (Layer == LayerToFind)
		{
			return Layer;
		}
	}

	return nullptr;
}

