// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#include "AmbiverseElement.h"

DEFINE_LOG_CATEGORY_CLASS(UAmbiverseElement, LogAmbiverseProceduralElement);

UAmbiverseElement* UAmbiverseElement::CreateInstanceFromRuntimeData(UObject* Outer, FAmbiverseElementRuntimeData& RuntimeData)
{
	if (!RuntimeData.ElementAsset) { return nullptr; }
	
	UAmbiverseElement* ElementInstance = {NewObject<UAmbiverseElement>(Outer)};
	
	ElementInstance->RuntimeData = RuntimeData;
	
	return ElementInstance;
}


