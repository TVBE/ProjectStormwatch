// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "InteractableObjectInterface.h"
#include "MeshInteractionComponent.generated.h"

UCLASS(NotBlueprintable, BlueprintType, Placeable, ClassGroup = ("Interaction"), Meta = (BlueprintSpawnableComponent))
class UMeshInteractionComponent : public UActorComponent, public IInteractableObject
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

public:
	FORCEINLINE virtual bool BeginInteraction(const EInteractionActionType Type, const AActor* Interactor) override { return false; }
	FORCEINLINE virtual bool EndInteraction(const EInteractionActionType Type, const AActor* Interactor) override { return false; }
	FORCEINLINE virtual EInteractionType GetInteractionType() const override { return InteractionType; }
	FORCEINLINE virtual EInteractionTriggerType GetInteractionTriggerType() const override { return InteractionTriggerType; }
	FORCEINLINE virtual EInteractionHandType GetInteractionHandType() const override { return InteractionHandType; }
	FORCEINLINE virtual FVector GetInteractionWidgetOffset() const override { return InteractionWidgetOffset; }

private:
	virtual void OnComponentCreated() override;
};
