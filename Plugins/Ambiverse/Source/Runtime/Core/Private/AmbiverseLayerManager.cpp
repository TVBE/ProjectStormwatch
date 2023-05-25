// Copyright (c) 2023-present Tim Verberne
// This source code is part of the Ambiverse plugin

#include "AmbiverseLayerManager.h"
#include "AmbiverseLayer.h"
#include "AmbiverseParameterManager.h"
#include "AmbiverseProceduralSoundData.h"
#include "AmbiverseSubsystem.h"

DEFINE_LOG_CATEGORY_CLASS(UAmbiverseLayerManager, LogAmbiverseLayerManager);

void UAmbiverseLayerManager::Initialize(UAmbiverseSubsystem* Subsystem)
{
	Super::Initialize(Subsystem);

	if (UAmbiverseParameterManager* ParameterManager {Subsystem->GetParameterManager()})
	{
		ParameterManager->OnParameterChangedDelegate.AddDynamic(this, &UAmbiverseLayerManager::HandleOnParameterChanged);
	}
}

void UAmbiverseLayerManager::RegisterAmbiverseLayer(UAmbiverseLayer* Layer)
{
	if (!Layer)
	{
		UE_LOG(LogAmbiverseLayerManager, Warning, TEXT("RegisterAmbiverseLayer: No Layer provided."));
		return;
	}

	if (Layer->ProceduralSounds.IsEmpty())
	{
		UE_LOG(LogAmbiverseLayerManager, Warning, TEXT("RegisterAmbiverseLayer: Layer has no procedural sounds: '%s'."),
		       *Layer->GetName());
		return;
	}

	for (FAmbiverseProceduralElementData& SoundData : Layer->ProceduralSounds)
	{
		FAmbiverseProceduralElementData::Validate(SoundData);
	}

	bool HasValidSoundData = false;
	for (const FAmbiverseProceduralElementData& SoundData : Layer->ProceduralSounds)
	{
		if (SoundData.IsValid)
		{
			HasValidSoundData = true;
			break;
		}
	}

	if (!HasValidSoundData)
	{
		UE_LOG(LogAmbiverseLayerManager, Warning,
		       TEXT("RegisterAmbiverseLayer: Layer has no valid procedural sounds: '%s'."), *Layer->GetName());
		return;
	}


	if (!FindActiveAmbienceLayer(Layer))
	{
		LayerRegistry.Add(Layer);
		Layer->InitializeSoundQueue();

		OnLayerRegistered.Broadcast(Layer);

		UE_LOG(LogAmbiverseLayerManager, Verbose, TEXT("Registered Ambiverse Layer: '%s'."), *Layer->GetName());
	}
}

void UAmbiverseLayerManager::UnregisterAmbiverseLayer(UAmbiverseLayer* Layer)
{
	if (!Layer)
	{
		UE_LOG(LogAmbiverseLayerManager, Warning, TEXT("UnregisterAmbiverseLayer: No Layer provided."));
		return;
	}
	if (LayerRegistry.Contains(Layer))
	{
		LayerRegistry.Remove(Layer);
		OnLayerUnregistered.Broadcast(Layer);

		UE_LOG(LogAmbiverseLayerManager, Verbose, TEXT("Unregistered Ambiverse Layer:: '%s'."), *Layer->GetName());
	}
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

void UAmbiverseLayerManager::HandleOnParameterChanged(UAmbiverseParameter* ChangedParameter)
{
	
}

void UAmbiverseLayerManager::Deinitialize(UAmbiverseSubsystem* Subsystem)
{
	if (!Subsystem) { return; }
	
	if (UAmbiverseParameterManager* ParameterManager {Subsystem->GetParameterManager()})
	{
		ParameterManager->OnParameterChangedDelegate.RemoveDynamic(this, &UAmbiverseLayerManager::HandleOnParameterChanged);
	}
	
	Super::Deinitialize(Subsystem);
}
