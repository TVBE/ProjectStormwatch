// Copyright (c) 2022-present Barrelhouse. All rights reserved.
// Written by Tim Verberne.

#pragma once

#include "CoreMinimal.h"
#include "InventoryObjectInterface.generated.h"

/** Defines whether the actor can be instantly added to the inventory by pressing the inventory action button,
 *	or if the player should be holding down the inventory action button for a while. */
UENUM(BlueprintType)
enum class EInventoryTriggerType : uint8
{
	SinglePress			UMETA(DisplayName = "Single Press"),
	PressAndHold		UMETA(DisplayName = "Press And Hold"),
};

UINTERFACE(Blueprintable, Meta = (DisplayName = "Inventory Object Interface"))
class FROSTBITE_API UInventoryObject : public UInterface
{
	GENERATED_BODY()
};

class FROSTBITE_API IInventoryObject
{
	GENERATED_BODY()

public:
	/** Returns whether the inventory object can currently be added to the inventory.
	 *	@Param Actor The actor that wants to add this item to its inventory.
	 *	@Return Whether the item can be added to an inventory.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory Object", Meta = (Displayname = "Can Be Added To Inventory"))
	bool CanAddToInventory(const AActor* Actor) const;
	
	/** Adds the item to the player's inventory.
	 *	@Param Actor The actor that owns this item when its added to an inventory.
	 *	@Return Whether the item was successfully added to the inventory.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory Object")
	bool AddToInventory(const AActor* Actor);

	/** Takes the item from the player's inventory.
	 *	@Param Actor The actor that takes this item from its inventory.
	 *	@Return Whether the item was successfully taken from the inventory.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory Object", Meta = (Displayname = "Remove From Inventory"))
	bool TakeFromInventory(const AActor* Actor);

	/** Returns the inventory trigger type of the object. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory Object", Meta = (Displayname = "Get Inventory Trigger Type"))
	EInventoryTriggerType GetTriggerType() const;
};