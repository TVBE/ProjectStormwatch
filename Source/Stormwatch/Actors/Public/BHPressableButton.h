// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ActorFunctionCaller.h"
#include "InteractableObjectInterface.h"
#include "TriggerableObjectInterface.h"
#include "UsableObjectInterface.h"
#include "GameFramework/Actor.h"
#include "BHPressableButton.generated.h"

class ABHPowerSource;
class UPowerConsumerComponent;
class UMeshCollisionTriggerComponent;
struct FTimerHandle;

/** Defines the trigger type of the button. */
UENUM(BlueprintType)
enum class EBHButtonTriggerType : uint8
{
	SinglePress				UMETA(DisplayName = "Single Press"),
	PressAndHold			UMETA(DisplayName = "Press and Hold"),
	Toggle					UMETA(DisplayName = "Toggle")
};

/** Defines an action for the button to perform if the power is lost. */
UENUM(BlueprintType)
enum class EBHButtonPowerChangeActionType : uint8
{
	Nothing					UMETA(DisplayName = "Nothing"),
	Release					UMETA(DisplayName = "Release If Pressed"),
	Press					UMETA(DisplayName = "Press If Pressed Before Power Outage")
};

/** Defines some actions to execute on linked buttons when the button is pressed or unpressed. */
UENUM(BlueprintType)
enum class EBHLinkedButtonAction : uint8
{
	Press			UMETA(DisplayName = "Press", Tooltip = "Presses the linked button when the action is triggered."),
	Release			UMETA(DisplayName = "Release", Tooltip = "Releases the linked button when the action is triggered."),
	Toggle			UMETA(DisplayName = "Toggle", Tooltip = "Inverts the linked button's state when the action is triggered."),
};


/** Struct containing a soft object pointer to another PressableButton actor,
 *	and actions to execute when the button that links the other PressableButton actor is pressed or released. */
USTRUCT(BlueprintType)
struct FBHLinkedButton
{
	GENERATED_BODY()

	/** The button actor that is linked.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Linked Button", Meta = (DisplayName = "Linked Button"))
	TSoftObjectPtr<ABHPressableButton> Actor;

	/** If true, an action should be triggered on the button when the button is pressed. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Linked Button", Meta = (DisplayName = "Do Action When Pressed", InlineEditConditionToggle))
	bool DoActionOnPress = false;

	/** The action that should be performed on the linked actor when the button is pressed. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Linked Button", Meta = (DisplayName = "Action When Pressed",
		EditCondition = "DoActionOnPress"))
	EBHLinkedButtonAction PressedAction = EBHLinkedButtonAction::Press;

	/** If true, an action should be triggered on the button when the button is released. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Linked Button", Meta = (DisplayName = "Do Action When Released", InlineEditConditionToggle))
	bool DoActionOnRelease = false;

	/** The action that should be performed on the linked actor when the button is released. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Linked Button", Meta = (DisplayName = "Action When Released",
		EditCondition = "DoActionOnRelease"))
	EBHLinkedButtonAction ReleasedAction = EBHLinkedButtonAction::Release;

	/** When true, the linked actor will not perform its gameplay action when triggered by this button. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Linked Button", Meta = (DisplayName = "Safe Link"))
	bool IsActionLinked = true;

	FBHLinkedButton()
	{
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnButtonPressedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnButtonReleasedDelegate);

/** Base class for button actors. */
UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "Interaction", Meta = (DisplayName = "Button"))
class ABHPressableButton : public AActor, public IInteractableObject, public IUsableObject
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogButton, Log, All)

public:
	/** Delegate that is called when the button is pressed. */
	UPROPERTY(BlueprintAssignable, Category = "Delegates", Meta = (DisplayName = "On Button Pressed"))
	FOnButtonPressedDelegate OnButtonPressed;

	/** Delegate that is called when the button is released. */
	UPROPERTY(BlueprintAssignable, Category = "Delegates", Meta = (DisplayName = "On Button Released"))
	FOnButtonReleasedDelegate OnButtonReleased;

protected:
	/** The root component for the actor. */
	UPROPERTY(BlueprintReadOnly, Category = "Components", Meta = (DisplayName = "Root"))
	USceneComponent* RootSceneComponent;
	
	/** The base static mesh for the button. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* BaseMesh;

	/** The button static mesh for the button. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* ButtonMesh;
	
	// GENERAL
	/** The trigger type of the button. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (DisplayName = "Type"))
	EBHButtonTriggerType TriggerType = EBHButtonTriggerType::SinglePress;
	
	/** The cooldown time between presses. When the button is in cooldown, the button cannot be pressed. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Button", Meta = (Units = "Seconds"))
	float CooldownTime = 1.0f;

	/** When true, the button is currently pressed. This does not mean that the player is currently interacting with the button,
	 *	just that the button is in it's 'pressed-down' state. */
	UPROPERTY(BlueprintReadWrite)
	bool IsPressed = false;
	
	// PHYSICS
	/** If true, the button can be triggered by colliding physics objects. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Button|Collision Trigger",
		Meta = (DisplayName = "Can Be Triggered By Collisions"))
	bool CanTriggerByCollision = true;

	/** The force threshold required to trigger the button. */ 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Button|Collision Trigger",
		Meta = (DisplayName = "Collision Trigger Threshold", Units = "Newtons", EditCondition = "CanTriggerByCollision"))
	float CollisionTriggerThreshold = 100.0f;
	
	/** The mesh collision trigger component. */
	UPROPERTY(BlueprintReadOnly, Category = "Components")
	UMeshCollisionTriggerComponent* CollisionTriggerComponent;
	
	// POWER
	/** If true, the button requires power to operate and can be connected to a power source. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Button|Power")
	bool RequiresPower = false;

	/** Soft object pointer to the power source that this button is connected to. */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Button|Power", Meta = (EditCondition = "RequiresPower"))
	TSoftObjectPtr<ABHPowerSource> PowerSource;

	/** If true, the button can still be toggled when the button has no power.
	 *	This will update the state, but will not trigger any gameplay actions or call the linked buttons. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Button|Power",
		Meta = (EditCondition = "RequiresPower && TriggerType == EBHButtonTriggerType::Toggle"))
	bool CanBeToggledWithoutPower = false;
	
	/** Defines the action the button should perform when power is lost. This is only available for toggle buttons. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly,  Category = "Button|Power", Meta = (DisplayName = "Action On Power Loss",
		EditCondition = "RequiresPower && TriggerType == EBHButtonTriggerType::Toggle",
		ValidEnumValues = "Nothing, Release"))
	EBHButtonPowerChangeActionType PowerLossAction = EBHButtonPowerChangeActionType::Nothing;

	/** Defines the action the button should perform when power is regained. This is only available for toggle buttons.  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Button|Power", Meta = (DisplayName = "Action On Power Gain",
		EditCondition = "RequiresPower && TriggerType == EBHButtonTriggerType::Toggle",
		ValidEnumValues = "Nothing, Press"))
	EBHButtonPowerChangeActionType PowerGainAction = EBHButtonPowerChangeActionType::Nothing;
	
	/** The power consumer component. */
	UPROPERTY(BlueprintReadOnly, Category = "Components")
	UPowerConsumerComponent* PowerConsumerComponent;

private:
	/** If true, the button is currently in cooldown and cannot be pressed. */
	UPROPERTY(BlueprintGetter = GetIsCooldownActive)
	bool IsCooldownActive = false;
	
	/** Timer handle for the cooldown timer. */
	UPROPERTY()
	FTimerHandle CooldownTimerHandle;

public:	
	ABHPressableButton();

	/** Presses the button. This function should not be called by the player: Implement the IInteractableObject interface instead.
	*	This functions should only be called by linked objects.
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Events", Meta = (DisplayName = "Press Button", Keywords = "Press Button"))
	void EventOnPress();

	/** Releases the button. This function should not be called by the player: Implement the IInteractableObject interface instead.
	 *	This functions should only be called by linked objects.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Events", Meta = (DisplayName = "Release Button", Keywords = "Release Button"))
	void EventOnRelease();

	/** IInteractable Object implementation. */
	FORCEINLINE FVector GetInteractionWidgetLocation_Implementation() const override { return GetActorLocation(); }

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	
	/** Starts the button's cooldown. */
	UFUNCTION(BlueprintCallable)
	void StartCooldown();

private:
	/** Called when the cooldown timer is finished. */
	void HandleCooldownFinished();

public:
	/** Event called when the button is triggered by a collision. */
	UFUNCTION(BlueprintNativeEvent, Category = "Events", Meta = (DisplayName = "On Collision Trigger"))
	void EventOnCollisionTrigger();

	/** Event called when the button's power state changes. */
	UFUNCTION(BlueprintNativeEvent, Category = "Events", Meta = (DisplayName = "On Power State Changed"))
	void EventOnPowerStateChanged(bool NewState);

	/** Returns whether the button is currently pressed. */
	UFUNCTION()
	FORCEINLINE bool GetIsPressed() const { return IsPressed; }
	
	/** Returns whether the button is currently in cooldown. */
	UFUNCTION(BlueprintGetter)
	FORCEINLINE bool GetIsCooldownActive() const { return IsCooldownActive; }
};
