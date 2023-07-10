// Copyright (c) 2022-present Tim Verberne
// This source code is part of the Adaptive Ambience System plugin.

#include "AmbiverseLayerFactory.h"
#include "AmbiverseLayerAsset.h"

UAmbiverseLayerFactory::UAmbiverseLayerFactory()
{
	SupportedClass = UAmbiverseLayerAsset::StaticClass();
	bCreateNew = true;
}

UObject* UAmbiverseLayerFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags,
	UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UAmbiverseLayerAsset>(InParent, Class, Name, Flags, Context);
}
