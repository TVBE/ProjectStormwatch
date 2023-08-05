// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "AmbiverseSceneFactory.generated.h"

UCLASS()
class AMBIVERSEEDITOR_API UAmbiverseSceneFactory : public UFactory
{
	GENERATED_BODY()

public:
	UAmbiverseSceneFactory();

	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};

