// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#include "AmbiverseLayerManager.h"
#include "AmbiverseLayer.h"
#include "AmbiverseParameterManager.h"
#include "AmbiverseProceduralElement.h"
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

void UAmbiverseLayerManager::Tick(const float DeltaTime)
{
	UpdateActiveLayers(DeltaTime);
}

void UAmbiverseLayerManager::UpdateActiveLayers(const float DeltaTime)
{
	if (ActiveLayers.IsEmpty()) { return; }

	for (UAmbiverseLayer* Layer : ActiveLayers)
	{
		UpdateElements(DeltaTime, Layer);

		Layer->ActiveDuration += DeltaTime;
		if (Layer->EnableLifetime)
		{
			if (Layer->Lifetime != 0.0f)
			{
				Layer->LifetimeRatio = Layer->ActiveDuration / Layer->Lifetime;
			}
			else
			{
				Layer->LifetimeRatio = 0.0f;
			}
		}
	}
}

void UAmbiverseLayerManager::UpdateElements(const float DeltaTime, UAmbiverseLayer* Layer)
{
	if (Layer->Elements.IsEmpty()) { return; }

	for (FAmbiverseProceduralElement& Element : Layer->Elements)
	{
		const float ScaleFactor {(Element.Time - DeltaTime) / Element.Time};
		Element.ReferenceTime *= ScaleFactor;

		Element.Time -= DeltaTime;

		if (Element.Time <= 0)
		{
			Owner->ProcessElement(Layer, Element);
		}
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

	for (FAmbiverseProceduralElement& SoundData : Layer->ProceduralSounds)
	{
		FAmbiverseProceduralElement::Validate(SoundData);
	}

	bool HasValidSoundData = false;
	for (const FAmbiverseProceduralElement& SoundData : Layer->ProceduralSounds)
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
		ActiveLayers.Add(Layer);
		Layer->BuildIndex();

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
	if (ActiveLayers.Contains(Layer))
	{
		ActiveLayers.Remove(Layer);
		OnLayerUnregistered.Broadcast(Layer);

		UE_LOG(LogAmbiverseLayerManager, Verbose, TEXT("Unregistered Ambiverse Layer:: '%s'."), *Layer->GetName());
	}
}

UAmbiverseLayer* UAmbiverseLayerManager::FindActiveAmbienceLayer(const UAmbiverseLayer* LayerToFind) const
{
	for (UAmbiverseLayer* Layer : ActiveLayers)
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


