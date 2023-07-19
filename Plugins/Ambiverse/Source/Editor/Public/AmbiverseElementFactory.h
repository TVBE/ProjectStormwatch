// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "AmbiverseElementFactory.generated.h"

UCLASS()
class AMBIVERSEEDITOR_API UAmbiverseElementFactory : public UFactory
{
	GENERATED_BODY()

public:
	UAmbiverseElementFactory();

	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};