// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PressableButton.generated.h"

struct FTimerHandle;

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

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Linked Button", Meta = (DisplayName = "Linked Button"))
	TSoftObjectPtr<APressableButton> Actor;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Linked Button", Meta = (DisplayName = "Action When Pressed"))
	ELinkedButtonAction PressedAction {ELinkedButtonAction::Release};

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Linked Button", Meta = (DisplayName = "Action When Released"))
	ELinkedButtonAction ReleasedAction {ELinkedButtonAction::Nothing};
};

/** Base class for button actors. Note that we do not implement the IInteractableObject interface here
 *	as some derived button types may not be pressable by the player. */
UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = (Interaction), Meta = (DisplayName = "Button"))
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
	TArray<TSoftObjectPtr<AActor>> TargetActors;
	
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

public:
	/** Presses the button. This function should not be called by the player: Implement the IInteractableObject interface instead.
	 *	This functions should only be called by linked objects. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Button", Meta = (DisplayName = "Press Button"))
	void EventOnPress();

	/** Releases the button. This function should not be called by the player: Implement the IInteractableObject interface instead.
	 *	This functions should only be called by linked objects. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Button", Meta = (DisplayName = "Release Button"))
	void EventOnRelease();

	/** Returns whether the button is currently pressed. */
	UFUNCTION()
	FORCEINLINE bool GetIsPressed() const { return IsPressed; }
	
	/** Returns whether the button is currently in cooldown. */
	UFUNCTION(BlueprintGetter, Category = "Button", Meta = (DisplayName = "Is Cooldown Active"))
	FORCEINLINE bool GetIsCooldownActive() const { return IsCooldownActive; }
};
