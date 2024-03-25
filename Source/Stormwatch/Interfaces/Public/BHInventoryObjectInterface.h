// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BHInventoryObjectInterface.generated.h"

UINTERFACE(Blueprintable, Meta = (DisplayName = "Inventory Object Interface"))
class STORMWATCH_API UBHInventoryObject : public UInterface
{
	GENERATED_BODY()
};

class STORMWATCH_API IBHInventoryObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BHInventoryObject")
	bool AddToInventory();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BHInventoryObject")
	bool RemoveFromInventory();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BHInventoryObject")
	bool CanAddToInventory() const;
};