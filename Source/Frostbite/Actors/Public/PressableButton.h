// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "TriggerableObjectInterface.h"
#include "GameFramework/Actor.h"
#include "PressableButton.generated.h"

struct FTimerHandle;

/** Struct containg a soft object pointer to an actor that implements the ITriggerableObject interface,
 *	and actions to execute when the button that targets the actor is pressed. */
USTRUCT(BlueprintType)
struct FButtonTargetActor
{
	GENERATED_BODY()

	/** The button actor that is linked.*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Linked Button", Meta = (DisplayName = "Target Actor"))
	TSoftObjectPtr<AActor> Actor;

	/** The action that should be performed on the linked actor when the button is pressed. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Linked Button", Meta = (DisplayName = "Action When Pressed"))
	ETriggerableObjectAction PressedAction {ETriggerableObjectAction::Trigger};

	/** The action that should be performed on the linked actor when the button is released. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Linked Button", Meta = (DisplayName = "Action When Released"))
	ETriggerableObjectAction ReleasedAction {ETriggerableObjectAction::Untrigger};
};

/** Defines some actions to execute on linked buttons when the button is pressed or unpressed. */
UENUM(BlueprintType)
enum class ELinkedButtonAction : uint8
{
	Press			UMETA(DisplayName = "Press", Tooltip = "Presses the linked button when the action is triggered."),
	Release			UMETA(DisplayName = "Release", Tooltip = "Releases the linked button when the action is triggered."),
	Toggle			UMETA(DisplayName = "Toggle", Tooltip = "Inverts the linked button's state when the action is triggered."),
	Custom			UMETA(DisplayName = "Custom"),
	Nothing			UMETA(DisplayName = "Nothing")
};

/** Struct containing a soft object pointer to another PressableButton actor,
 *	and actions to execute when the button that links the other PressableButton actor is pressed or released. */
USTRUCT(BlueprintType)
struct FLinkedButton
{
	GENERATED_BODY()

	/** The button actor that is linked.*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Linked Button", Meta = (DisplayName = "Linked Button"))
	TSoftObjectPtr<APressableButton> Actor;

	/** The action that should be performed on the linked actor when the button is pressed. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Linked Button", Meta = (DisplayName = "Action When Pressed"))
	ELinkedButtonAction PressedAction {ELinkedButtonAction::Release};

	/** The action that should be performed on the linked actor when the button is released. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Linked Button", Meta = (DisplayName = "Action When Released"))
	ELinkedButtonAction ReleasedAction {ELinkedButtonAction::Nothing};

	/** When true, the linked actor will not perform its gameplay action when triggered by this button. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Linked Button", Meta = (DisplayName = "Safe Link"))
	bool IsActionLinked {true};
};

/** Base class for button actors. Note that we do not implement the IInteractableObject interface here
 *	as some derived button types may not be pressable by the player. */
UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = Interaction, Meta = (DisplayName = "Button"))
class APressableButton : public AActor
{
	GENERATED_BODY()

protected:
	/** The cooldown time between presses. When the button is in cooldown, the button cannot be pressed. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Button", Meta = (DisplayName = "Cooldown Time"))
	float CooldownTime {1.0f};

	/** When true, the button is currently pressed. This does not mean that the player is currently interacting with the button,
	 *	just that the button is in it's 'pressed-down' state. */
	UPROPERTY(BlueprintReadWrite, Category = "Button", Meta = (DisplayName = "Is Pressed"))
	bool IsPressed {false};

	/** Array of soft object pointers to other button instances. This allows us to affect other button instances when this button is pressed. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Button|Linked Actors", Meta = (DisplayName = "Target Actors"))
	TArray<FButtonTargetActor> TargetActors;
	
	/** Array of soft object pointers to other button instances. This allows us to affect other button instances when this button is pressed. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Button|Linked Actors", Meta = (DisplayName = "Linked Buttons"))
	TArray<FLinkedButton> LinkedButtons;

private:
	/** If true, the button is currently in cooldown and cannot be pressed. */
	UPROPERTY(BlueprintGetter=GetIsCooldownActive, Category = "Button", Meta = (DisplayName = "Is Cooldown Active"))
	bool IsCooldownActive {false};
	
	/** Timer handle for the cooldown timer. */
	UPROPERTY()
	FTimerHandle CooldownTimerHandle;

public:	
	APressableButton();

protected:
	virtual void BeginPlay() override;

	/** Starts the button's cooldown. */
	UFUNCTION(BlueprintCallable, Category = "Button", Meta = (DisplayName = "Start Cooldown", BlueprintProtected))
	void StartCooldown();

private:
	/** Called when the cooldown timer is finished. */
	void HandleCooldownFinished();

	/** calls the corresponding actions on target actors. */
	void DoTargetActorActions(const bool IsPressedAction);

	/** Calls the corresponding actions on linked button instances. */
	void DoLinkedButtonActions(const bool IsPressedAction);

public:
	/** Presses the button. This function should not be called by the player: Implement the IInteractableObject interface instead.
	 *	This functions should only be called by linked objects.
	 *	@Param CallTargetActorss If true, the event should execute its gameplay actions.
	 *	@Param CallLinkedButtons If true, the button should perform the specified actions on other buttons.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Button", Meta = (DisplayName = "Press Button"))
	void EventOnPress(const bool CallTargetActors = true, const bool CallLinkedButtons = true);

	/** Releases the button. This function should not be called by the player: Implement the IInteractableObject interface instead.
	 *	This functions should only be called by linked objects.
	 *	@Param CallTargetActors If true, the event should execute its gameplay actions.
	 *	@Param CallLinkedButtons If true, the button should perform the specified actions on other buttons.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Button", Meta = (DisplayName = "Release Button"))
	void EventOnRelease(const bool CallTargetActors = true, const bool CallLinkedButtons = true);

	/** Returns whether the button is currently pressed. */
	UFUNCTION()
	FORCEINLINE bool GetIsPressed() const { return IsPressed; }
	
	/** Returns whether the button is currently in cooldown. */
	UFUNCTION(BlueprintGetter, Category = "Button", Meta = (DisplayName = "Is Cooldown Active"))
	FORCEINLINE bool GetIsCooldownActive() const { return IsCooldownActive; }
};
