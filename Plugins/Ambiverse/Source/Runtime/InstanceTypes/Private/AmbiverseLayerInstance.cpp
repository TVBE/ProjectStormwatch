// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#include "AmbiverseLayerInstance.h"
#include "AmbiverseElementRuntimeData.h"
#include "AmbiverseElementManager.h"
#include "AmbiverseElementInstance.h"
#include "AmbiverseSubsystem.h"

bool UAmbiverseLayerInstance::Initialize(UAmbiverseSubsystem* Subsystem)
{
	if (!Subsystem) { return false; }
	AmbiverseSubsystem = Subsystem;
	
	if (Asset->Elements.Num() > 0)
	{
		Asset->Elements.RemoveAll([](const FAmbiverseElementRuntimeData& Element){ return !Element.IsValid(); });
		
		for(const FAmbiverseElementRuntimeData& RuntimeElementData : Asset->Elements)
		{
			UAmbiverseElementInstance* NewRuntimeElement = NewObject<UAmbiverseElementInstance>(this, UAmbiverseElementInstance::StaticClass());
			
			NewRuntimeElement->RuntimeData = RuntimeElementData;
			
			this->ProceduralElements.Add(NewRuntimeElement);
		}
		
		if (UAmbiverseElementManager* ElementManager {AmbiverseSubsystem->GetElementManager()})
		{
			ElementManager->RegisterElements(ProceduralElements);
		}
		else
		{
			return false;
		}

		IsInitialized = true;
	}
	else
	{
		IsInitialized = false;
	}
	return IsInitialized;
}

UAmbiverseLayerInstance* UAmbiverseLayerInstance::CreateInstanceFromAsset(UObject* Outer, UAmbiverseLayerAsset* Asset)
{
	if (!Asset) { return nullptr; }
	
	UAmbiverseLayerInstance* LayerInstance = {NewObject<UAmbiverseLayerInstance>(Outer)};
	
	LayerInstance->Asset = Asset;
	
	return LayerInstance;
}
