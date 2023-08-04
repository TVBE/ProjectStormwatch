// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#include "AmbiverseLayer.h"
#include "AmbiverseElementRuntimeData.h"
#include "AmbiverseSoundscapeManager.h"
#include "AmbiverseElement.h"
#include "AmbiverseSubsystem.h"

bool UAmbiverseLayer::Initialize(UAmbiverseSubsystem* Subsystem)
{
	if (!Subsystem) { return false; }
	AmbiverseSubsystem = Subsystem;
	
	if (Asset->Elements.Num() > 0)
	{
		Asset->Elements.RemoveAll([](const FAmbiverseElementRuntimeData& Element){ return !Element.IsValid(); });
		
		for(const FAmbiverseElementRuntimeData& RuntimeElementData : Asset->Elements)
		{
			UAmbiverseElement* NewRuntimeElement = NewObject<UAmbiverseElement>(this, UAmbiverseElement::StaticClass());
			
			NewRuntimeElement->RuntimeData = RuntimeElementData;
			
			this->ProceduralElements.Add(NewRuntimeElement);
		}
		
		if (UAmbiverseSoundscapeManager* ElementManager {AmbiverseSubsystem->GetSoundscapeManager()})
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

UAmbiverseLayer* UAmbiverseLayer::CreateInstanceFromAsset(UObject* Outer, UAmbiverseLayerAsset* Asset)
{
	if (!Asset) { return nullptr; }
	
	UAmbiverseLayer* LayerInstance = {NewObject<UAmbiverseLayer>(Outer)};
	
	LayerInstance->Asset = Asset;
	
	return LayerInstance;
}
