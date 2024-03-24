// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BHDestructibleObjectInterface.generated.h"


UINTERFACE(Blueprintable)
class UBHDestructibleObject : public UInterface
{
	GENERATED_BODY()
};

class IBHDestructibleObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BHDestructibleObject")
	bool Break();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BHDestructibleObject")
	bool Repair();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BHDestructibleObject")
	bool IsBroken() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BHDestructibleObject")
	float GetIntegrity() const;
};

