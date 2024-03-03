// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#include "AmbiverseElementInstance.h"

DEFINE_LOG_CATEGORY_CLASS(UAmbiverseElementInstance, LogAmbiverseProceduralElement);

UAmbiverseElementInstance* UAmbiverseElementInstance::CreateInstanceFromRuntimeData(UObject* Outer, FAmbiverseElementRuntimeData& RuntimeData)
{
	if (!RuntimeData.ElementAsset) { return nullptr; }
	
	UAmbiverseElementInstance* ElementInstance = {NewObject<UAmbiverseElementInstance>(Outer)};
	
	ElementInstance->RuntimeData = RuntimeData;
	
	return ElementInstance;
}


