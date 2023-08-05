// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#include "AmbiverseSceneFactory.h"
#include "AmbiverseSceneAsset.h"

UAmbiverseSceneFactory::UAmbiverseSceneFactory()
{
	SupportedClass = UAmbiverseSceneAsset::StaticClass();
	bCreateNew = true;
}

UObject* UAmbiverseSceneFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags,
	UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UAmbiverseSceneAsset>(InParent, Class, Name, Flags, Context);
}
