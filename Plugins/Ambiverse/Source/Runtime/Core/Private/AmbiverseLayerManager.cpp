// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#include "AmbiverseLayerManager.h"
#include "AmbiverseComposite.h"
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
	if (Layer->ProceduralElements.IsEmpty()) { return; }

	for (FAmbiverseProceduralElement& Element : Layer->ProceduralElements)
	{
		const float ScaleFactor {(Element.Time - DeltaTime) / Element.Time};
		Element.ReferenceTime *= ScaleFactor;

		Element.Time -= DeltaTime;

		if (Element.Time <= 0)
		{
			Owner->ProcessProceduralElement(Layer, Element);
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

	if (Layer->ProceduralElements.IsEmpty())
	{
		UE_LOG(LogAmbiverseLayerManager, Warning, TEXT("RegisterAmbiverseLayer: Layer has no procedural sounds: '%s'."),
		       *Layer->GetName());
		return;
	}
	
	bool HasValidSoundData = false;
	for (const FAmbiverseProceduralElement& ProceduralElement : Layer->ProceduralElements)
	{
		if (ProceduralElement.IsValid())
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
		Layer->InitializeLayer();

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

void UAmbiverseLayerManager::RegisterAmbiverseComposite(UAmbiverseComposite* Composite)
{
	if (!Composite)
	{
		UE_LOG(LogTemp, Warning, TEXT("RegisterAmbiverseComposite: No Composite provided."));
		return;
	}

	if (Composite->StopNonCompositeLayers)
	{
		for (UAmbiverseLayer* Layer : ActiveLayers)
		{
			if (!Composite->Layers.Contains(Layer))
			{
				UnregisterAmbiverseLayer(Layer);
			}
		}
	}

	for (UAmbiverseLayer* Layer : Composite->Layers)
	{
		RegisterAmbiverseLayer(Layer);
	}
}

void UAmbiverseLayerManager::UnregisterAmbiverseComposite(UAmbiverseComposite* Composite)
{
	if (!Composite)
	{
		UE_LOG(LogTemp, Warning, TEXT("UnregisterAmbiverseComposite: No Composite provided."));
		return;
	}

	for (UAmbiverseLayer* Layer : Composite->Layers)
	{
		UnregisterAmbiverseLayer(Layer);
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


