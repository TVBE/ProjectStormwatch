// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "InteractableObjectInterface.h"
#include "InventoryObjectInterface.h"
#include "MeshInteractionComponent.generated.h"

UCLASS(NotBlueprintable, BlueprintType, Placeable, ClassGroup = ("Interaction"), Meta = (BlueprintSpawnableComponent))
class UMeshInteractionComponent : public UActorComponent, public IInteractableObject, public IInventoryObject
{
	GENERATED_BODY()

private:
	/** When true, the InteractionType and InterationHandType will be set automatically according according to the meshes weight and size. */
	UPROPERTY(EditInstanceOnly, Category = "Interaction", Meta = (DisplayName = "Set Automatically"))
	bool IsAutoConfigurable {true};
	
	/** When true, the mesh is draggable instead of grabbable. */
	UPROPERTY(EditInstanceOnly, Category = "Interaction", Meta = (DisplayName = "Is Heavy", EditCondition = "!IsAutoConfigurable", EditConditionHides))
	bool IsHeavy {false};

	/** When true, the mesh requires two hands to interact with instead of one. */
	UPROPERTY(EditInstanceOnly, Category = "Interaction", Meta = (DisplayName = "Is Large", EditCondition = "!IsAutoConfigurable", EditConditionHides))
	bool IsLarge {false};
	
	/** The interaction type of this interactable static mesh actor. */
	UPROPERTY()
	EInteractionType InteractionType {EInteractionType::Grabbable};

	/** The interaction trigger type of this interactable static mesh actor. */
	UPROPERTY()
	EInteractionTriggerType InteractionTriggerType {EInteractionTriggerType::SinglePress};

	/** The interaction trigger type of this interactable static mesh actor. */
	UPROPERTY()
	EInteractionHandType InteractionHandType {EInteractionHandType::OneHanded};

	/** The Offset of the interaction widget. */
	UPROPERTY(EditInstanceOnly, Category = "Interaction", Meta = (DisplayName = "Widget Offset"))
	FVector InteractionWidgetOffset {FVector()};

	/** If true, the CanAddToInventory boolean is not set automatically, but has to be configured manually instead. */
	UPROPERTY(EditInstanceOnly, Category = "Inventory", Meta = (DisplayName = "Override Automatic Inventory Configuration"))
	bool OverrideInventoryAutoConfig {false};
	
	/** If true, the mesh can be added to the player's inventory. This will be set to false if the object is too heavy or too large. */
	UPROPERTY(EditInstanceOnly, Category = "Inventory", Meta = (DisplayName = "Can Be Added To Inventory",
		EditCondition = OverrideInventoryAutoConfig, InlineEditConditionToggle))
	bool IsInventoryAddable {false};

	/** Determines whether this actor can be added to the inventory with a single press of the inventory action, or whether the action is press-and-hold. */
	UPROPERTY(EditInstanceOnly, Category = "Inventory", Meta = (DisplayName = "Inventory Trigger Type"))
	EInventoryTriggerType InventoryTriggerType {EInventoryTriggerType::PressAndHold};

public:
	/** IInteractableObject interface functions. */
	FORCEINLINE bool Use_Implementation(const AActor* Interactor) override { return false; }
	FORCEINLINE bool Disuse_Implementation(const AActor* Interactor) override { return false; }
	FORCEINLINE EInteractionType GetInteractionType_Implementation() const override { return InteractionType; }
	FORCEINLINE EInteractionTriggerType GetInteractionTriggerType_Implementation() const override { return InteractionTriggerType; }
	FORCEINLINE EInteractionHandType GetInteractionHandType_Implementation() const override { return InteractionHandType; }
	FORCEINLINE FVector GetInteractionWidgetOffset_Implementation() const override { return InteractionWidgetOffset; }

	/** IInventoryObject interface functions. */
	FORCEINLINE bool CanAddToInventory_Implementation(const AActor* Actor) const override { return IsInventoryAddable; };
	bool AddToInventory_Implementation(const AActor* Actor) override;
	bool TakeFromInventory_Implementation(const AActor* Actor) override;
	FORCEINLINE EInventoryTriggerType GetTriggerType_Implementation() const override { return InventoryTriggerType; }

private:
	virtual void OnComponentCreated() override;
	virtual void OnRegister() override;

	/** Determines whether the mesh can be added to the inventory or not. */
	bool DetermineInventoryAddibility() const;
};
