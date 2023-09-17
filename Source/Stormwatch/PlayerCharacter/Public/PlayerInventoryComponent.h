// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne & Nino Saglia
// This source code is part of the project Stormwatch

#pragma once

#include "CoreMinimal.h"
#include "StormwatchMacros.h"
#include "Components/ActorComponent.h"
#include "PlayerInventoryComponent.generated.h"

class UPlayerInteractionComponent;

UCLASS(Blueprintable, BlueprintType, ClassGroup = "PlayerCharacter",
	   Meta = (BlueprintSpawnableComponent, DisplayName = "Player Inventory Component"))
class STORMWATCH_API UPlayerInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

	PLAYER_COMPONENT_BODY()

private:
	/** The currently selected slot in the Inventory. */
	UPROPERTY(BlueprintGetter = GetCurrentSelectedSlotIndex)
	int32 SelectedSlot {0};

	/** The actor in the hotbar at the index of the selected slot. */
	UPROPERTY(BlueprintGetter = GetCurrentSelectedSlotActor)
	AActor* SelectedActor;

	/** The amount of hotbar slots available for the inventory. */
	UPROPERTY(BlueprintGetter = GetHotbarSize, EditDefaultsOnly, Category = "PlayerInventoryComponent",
			  Meta = (DisplayName = "Hotbar Slots", ClampMin = "0", ClampMax = "10", UIMin = "0", UIMax = "10"))
	int32 HotbarSlots {4};

	/** The entry of AActor pointers in the hotbar. This property can be treated as 'the hotbar'. */
	UPROPERTY(BlueprintGetter = GetHotbar)
	TArray<AActor*> Hotbar;

public:
	UPlayerInventoryComponent();

	/** Checks if an actor or one of its components implements the IInventoryObject interface.
	*	Returns the first UObject that implements the interface that it finds. */
	UFUNCTION(BlueprintPure, Category = "PlayerInventoryComponent", Meta = (DisplayName = "Find Inventory Object"))
	UObject* FindInventoryObject(AActor* Actor) const;

	/** Tries to add an item into the hotbar. */
	UFUNCTION(BlueprintCallable, Category = "PlayerInventoryComponent", Meta = (DisplayName = "Add Object To Inventory"))
	bool AddActorToInventory(AActor* Actor);

	/** Tries to take an item from the hotbar. */
	UFUNCTION(BlueprintCallable, Category = "PlayerInventoryComponent", Meta = (DisplayName = "Take Actor From Inventory"))
	AActor* TakeActorFromInventory();

protected:
	virtual void BeginPlay() override;
	virtual void OnComponentCreated() override;

private:
	/** Called when the interaction component has detected a new interactable actor in front of the player. */
	UFUNCTION()
	void HandleInteractableActorChanged(AActor* InteractableActor);

	/** Tries to find a component that implements the IInventoryObject interface in a specified actor.*/
	UFUNCTION()
	UActorComponent* FindInventoryComponent(const AActor* Actor) const;

public:
	/** Returns the current selected slot index. */
	UFUNCTION(BlueprintGetter, Category = "PlayerInventoryComponent", Meta = (DisplayName = "Get Current Selected Hotbar Slot Index"))
	int32 GetCurrentSelectedSlotIndex() const { return SelectedSlot; }

	/** Returns the current selected slot actor. */
	UFUNCTION(BlueprintGetter, Category = "PlayerInventoryComponent", Meta = (DisplayName = "Get Current Selected Hotbar Slot Actor"))
	AActor* GetCurrentSelectedSlotActor() const { return SelectedActor; }

	/** Returns the hotbar size. */
	UFUNCTION(BlueprintGetter, Category = "PlayerInventoryComponent", Meta = (DisplayName = "Get Hotbar Size"))
	int32 GetHotbarSize() const { return HotbarSlots; }

	/** Returns the hotbar. */
	UFUNCTION(BlueprintGetter, Category = "PlayerInventoryComponent", Meta = (DisplayName = "Get Hotbar"))
	TArray<AActor*> GetHotbar() const { return Hotbar; }
};
