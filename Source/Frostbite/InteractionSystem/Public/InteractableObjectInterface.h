// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "InteractableObjectInterface.generated.h"

/** Defines the interaction type of an object. */
UENUM(BlueprintType)
enum class EInteractionType : uint8
{
	Grabbable			UMETA(DisplayName = "Grabbable", Tooltip = "Grabbable actors can only be grabbed, rotated, and thrown. They don't offer any additional functionality."),
	Draggable			UMETA(DisplayName = "Draggable", Tooltip = "Draggable actors can only be dragged or pushed. They don't offer any additional functionality."),
	Usable				UMETA(DisplayName = "Usable", Tooltip = "Usable items can be interacted with using the secondary interaction action. However, they cannot be grabbed."),
	Handleable			UMETA(DisplayName = "Handleable", Tooltip = "Handleable items can both be grabbed and used."),
	GrabUsable			UMETA(DisplayName = "Grab Usable", Tooltip = "Grab Usable actors are actors that serve the purpose of Usable actors, but are 'used' by using the grab action.")
};

/** Defines whether the actor has a single-press interaction, or a press-and-hold interaction.
 *	This is only really relevant for usable and handleable objects. */
UENUM(BlueprintType)
enum class EInteractionTriggerType : uint8
{
	SinglePress			UMETA(DisplayName = "Single Press"),
	PressAndHold		UMETA(DisplayName = "Press And Hold"),
};

/** Defines whether the actor requires one or two hands to perform the interaction. */
UENUM(BlueprintType)
enum class EInteractionHandType : uint8
{
	OneHanded			UMETA(DisplayName = "One Handed"),
	TwoHanded			UMETA(DisplayName = "Two Handed"),
};

/** C++ implementable interface. */
UINTERFACE(Blueprintable)
class FROSTBITE_API UInteractableObject : public UInterface
{
	GENERATED_BODY()
};

class FROSTBITE_API IInteractableObject
{
	GENERATED_BODY()

public:
	/** Begins interaction with the object.
	 *	@Param Instigator The actor that instigated the interaction.
	 *	@Return Whether the object implements an interaction.
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "InteractableObject", Meta = (DisplayName = "Begin Interaction"))
	bool BeginInteraction(const EInteractionActionType Type, const AActor* Interactor);

	/** Ends interaction with the object.
	 *	@Param Instigator The actor that instigated the interaction.
	 *	@Return Whether the object implements an interaction.
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "InteractableObject", Meta = (DisplayName = "End Interaction"))
	bool EndInteraction(const EInteractionActionType Type, const AActor* Interactor);

	/** Returns the interaction type of the object. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "InteractableObject", Meta = (DisplayName = "Get Interaction Type"))
	EInteractionType GetInteractionType() const;

	/** Returns whether the object has a single-press interaction, or a press-and-hold interaction. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "InteractableObject", Meta = (DisplayName = "Get Interaction Trigger Type"))
	EInteractionTriggerType GetInteractionTriggerType() const;
	
	/** Returns the interaction hand type of the object. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "InteractableObject", Meta = (DisplayName = "Get Interaction Hand Type"))
	EInteractionHandType GetInteractionHandType() const;

	/** Returns the offset of the interaction widget. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "InteractableObject", Meta = (DisplayName = "Interaction Widget Offset"))
	FVector GetInteractionWidgetOffset() const;
};
