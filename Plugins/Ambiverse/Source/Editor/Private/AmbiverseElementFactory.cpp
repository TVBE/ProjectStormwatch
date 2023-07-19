// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#include "AmbiverseElementFactory.h"
#include "AmbiverseElementAsset.h"

UAmbiverseElementFactory::UAmbiverseElementFactory()
{
	SupportedClass = UAmbiverseElementAsset::StaticClass();
	bCreateNew = true;
}

UObject* UAmbiverseElementFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags,
	UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UAmbiverseElementAsset>(InParent, Class, Name, Flags, Context);
}