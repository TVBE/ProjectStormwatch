// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#include "AmbiverseLayerManager.h"
#include "AmbiverseCompositeAsset.h"
#include "AmbiverseLayerAsset.h"
#include "AmbiverseLayerInstance.h"
#include "AmbiverseParameterManager.h"
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
}

bool UAmbiverseLayerManager::RegisterAmbiverseLayer(UAmbiverseLayerAsset* LayerAsset)
{
	if (!LayerAsset)
	{
		UE_LOG(LogAmbiverseLayerManager, Warning, TEXT("RegisterAmbiverseLayer: No Layer provided."));
		return false;
	}
	
	if (LayerAsset->Elements.IsEmpty())
	{
		UE_LOG(LogAmbiverseLayerManager, Warning, TEXT("RegisterAmbiverseLayer: Layer has no procedural sounds: '%s'."),
		       *LayerAsset->GetName());
		return false;
	}
	
	if (!FindInstanceByAsset(LayerAsset))
	{
		UAmbiverseLayerInstance* LayerInstance {UAmbiverseLayerInstance::CreateInstanceFromAsset(Owner, LayerAsset)};
		LayerInstance->Initialize(Owner);
		ActiveLayers.Add(LayerInstance);
		
		OnLayerRegistered.Broadcast(LayerAsset);

		UE_LOG(LogAmbiverseLayerManager, Verbose, TEXT("Registered Ambiverse Layer: '%s'."), *LayerAsset->GetName());

		return true;
		
	}
	return false;
}

bool UAmbiverseLayerManager::UnregisterAmbiverseLayer(UAmbiverseLayerAsset* LayerAsset, const bool ForceStop, const float FadeTime)
{
	if (!LayerAsset)
	{
		UE_LOG(LogAmbiverseLayerManager, Warning, TEXT("UnregisterAmbiverseLayer: No LayerAsset provided."));
		return false;
	}

	if (UAmbiverseLayerInstance* LayerInstance {FindInstanceByAsset(LayerAsset)})
	{
		ActiveLayers.Remove(LayerInstance);
		OnLayerUnregistered.Broadcast(LayerAsset);

		UE_LOG(LogAmbiverseLayerManager, Verbose, TEXT("Unregistered Ambiverse Layer: '%s'."), *LayerAsset->GetName());
		return true;
	}
	return false;
}

UAmbiverseLayerInstance* UAmbiverseLayerManager::FindInstanceByAsset(UAmbiverseLayerAsset* Asset) const
{
	for (UAmbiverseLayerInstance* LayerInstance : ActiveLayers)
	{
		if (LayerInstance && LayerInstance->Asset == Asset)
		{
			return LayerInstance;
		}
	}

	return nullptr;
}

void UAmbiverseLayerManager::HandleOnParameterChanged(UAmbiverseParameterAsset* ChangedParameter)
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


