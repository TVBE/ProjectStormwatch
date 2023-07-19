// Copyright (c) 2023-present Tim Verberne. All rights reserved.

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
