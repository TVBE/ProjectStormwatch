// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#include "AmbiverseCompositeFactory.h"
#include "AmbiverseCompositeAsset.h"

UAmbiverseCompositeFactory::UAmbiverseCompositeFactory()
{
	SupportedClass = UAmbiverseCompositeAsset::StaticClass();
	bCreateNew = true;
}

UObject* UAmbiverseCompositeFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags,
	UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UAmbiverseCompositeAsset>(InParent, Class, Name, Flags, Context);
}