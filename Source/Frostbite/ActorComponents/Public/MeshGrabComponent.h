// Copyright (c) 2022-present Barrelhouse. All rights reserved.
// Written by Tim Verberne.

#pragma once

#include "CoreMinimal.h"
#include "GrabbableObjectInterface.h"
#include "InventoryObjectInterface.h"
#include "MeshInteractionComponent.h"
#include "MeshGrabComponent.generated.h"

UCLASS(NotBlueprintable, BlueprintType, ClassGroup = "Interaction", Meta = (BlueprintSpawnableComponent,
	DisplayName = "Mesh Grab Component", ShortToolTip = "Component that allows the player to grab and throw static mesh and components."))
class UMeshGrabComponent : public UMeshInteractionComponent, public IGrabbableObject, public IInventoryObject
{
	GENERATED_BODY()

private:
	bool OverrideInventoryAutoConfig {false};
	
	/** If true, the mesh can be added to the player's inventory. This will be set to false if the object is too heavy or too large. */
	UPROPERTY(EditInstanceOnly, Category = "Inventory", Meta = (DisplayName = "Can Be Added To Inventory",
		EditCondition = OverrideInventoryAutoConfig, InlineEditConditionToggle))
	bool IsInventoryAddable {false};

	/** Determines whether this actor can be added to the inventory with a single press of the inventory action, or whether the action is press-and-hold. */
	UPROPERTY(EditInstanceOnly, Category = "Inventory", Meta = (DisplayName = "Inventory Trigger Type"))
	EInventoryTriggerType InventoryTriggerType {EInventoryTriggerType::PressAndHold};

public:
	/** IGrabbableObject interface functions. */
	FORCEINLINE bool BeginGrab_Implementation(const AActor* Interactor) override { return false; }
	FORCEINLINE bool EndGrab_Implementation(const AActor* Interactor) override { return false; }
	FORCEINLINE EGrabType GetGrabType_Implementation() const override { return EGrabType::OneHanded; }

	/** IInventoryObject interface functions. */
	FORCEINLINE bool CanAddToInventory_Implementation(const AActor* Actor) const override { return IsInventoryAddable; };
	bool AddToInventory_Implementation(const AActor* Actor) override;
	bool TakeFromInventory_Implementation(const AActor* Actor) override;
	FORCEINLINE EInventoryTriggerType GetTriggerType_Implementation() const override { return InventoryTriggerType; }

private:
	virtual void OnRegister() override;
};
