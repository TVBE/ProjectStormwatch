// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "InventoryObjectInterface.generated.h"

UINTERFACE(MinimalAPI)
class UInventoryItem : public UInterface
{
	GENERATED_BODY()
};

class FROSTBITE_API IInventoryItem
{
	GENERATED_BODY()

public:
	/** returns whether the item can be added to the inventory. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
	bool CanAddToInventory();

	/* Returns whether the item is a consumable. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
	bool IsConsumableItem();

	/** Uses the item. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
	void UseItem();
};