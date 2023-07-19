// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#include "AmbiverseParameterFactory.h"
#include "AmbiverseParameterAsset.h"

UAmbiverseParameterFactory::UAmbiverseParameterFactory()
{
	SupportedClass = UAmbiverseParameterAsset::StaticClass();
	bCreateNew = true;
}

UObject* UAmbiverseParameterFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags,
	UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UAmbiverseParameterAsset>(InParent, Class, Name, Flags, Context);
}