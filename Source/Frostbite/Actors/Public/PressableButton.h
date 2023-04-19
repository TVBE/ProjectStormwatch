// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "TriggerableObjectInterface.h"
#include "UsableObjectInterface.h"
#include "GameFramework/Actor.h"
#include "PressableButton.generated.h"

class APowerSource;
class UPowerConsumerComponent;
class UMeshCollisionTriggerComponent;
struct FTimerHandle;

/** Defines the trigger type of the button. */
UENUM(BlueprintType)
enum class EButtonTriggerType : uint8
{
	SinglePress				UMETA(DisplayName = "Single Press"),
	PressAndHold			UMETA(DisplayName = "Press and Hold"),
	Toggle					UMETA(DisplayName = "Toggle")
};

/** Defines an action for the button to perform if the power is lost. */
UENUM(BlueprintType)
enum class EButtonPowerChangeActionType : uint8
{
	Nothing					UMETA(DisplayName = "Nothing"),
	Release					UMETA(DisplayName = "Release If Pressed"),
	Press					UMETA(DisplayName = "Press If Pressed Before Power Outage")
};

/** Defines some actions to execute on linked buttons when the button is pressed or unpressed. */
UENUM(BlueprintType)
enum class ELinkedButtonAction : uint8
{
	Press			UMETA(DisplayName = "Press", Tooltip = "Presses the linked button when the action is triggered."),
	Release			UMETA(DisplayName = "Release", Tooltip = "Releases the linked button when the action is triggered."),
	Toggle			UMETA(DisplayName = "Toggle", Tooltip = "Inverts the linked button's state when the action is triggered."),
};

/** Struct containg a soft object pointer to an actor that implements the ITriggerableObject interface,
 *	and actions to execute when the button that targets the actor is pressed. */
USTRUCT(BlueprintType)
struct FButtonTargetActor
{
	GENERATED_BODY()

	/** The button actor that is linked.*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Linked Button", Meta = (DisplayName = "Target Actor"))
	TSoftObjectPtr<AActor> Actor;
	
	/** If true, an action should be triggered on the button when the button is pressed. */
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Linked Button", Meta = (DisplayName = "Do Action When Pressed", InlineEditConditionToggle))
    bool DoActionOnPress {false};

	/** The action that should be performed on the linked actor when the button is pressed. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Linked Button", Meta = (DisplayName = "Action When Pressed",
		EditCondition = "DoActionOnPress"))
	ETriggerableObjectAction PressedAction {ETriggerableObjectAction::Trigger};

	/** If true, an action should be triggered on the button when the button is released. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Linked Button", Meta = (DisplayName = "Do Action When Released", InlineEditConditionToggle))
	bool DoActionOnRelease {false};

	/** The action that should be performed on the linked actor when the button is released. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Linked Button", Meta = (DisplayName = "Action When Released", 
	EditCondition = "DoActionOnRelease"))
	ETriggerableObjectAction ReleasedAction {ETriggerableObjectAction::Untrigger};
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

	/** If true, an action should be triggered on the button when the button is pressed. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Linked Button", Meta = (DisplayName = "Do Action When Pressed", InlineEditConditionToggle))
	bool DoActionOnPress {false};

	/** The action that should be performed on the linked actor when the button is pressed. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Linked Button", Meta = (DisplayName = "Action When Pressed",
		EditCondition = "DoActionOnPress"))
	ELinkedButtonAction PressedAction {ELinkedButtonAction::Press};

	/** If true, an action should be triggered on the button when the button is released. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Linked Button", Meta = (DisplayName = "Do Action When Released", InlineEditConditionToggle))
	bool DoActionOnRelease {false};

	/** The action that should be performed on the linked actor when the button is released. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Linked Button", Meta = (DisplayName = "Action When Released",
		EditCondition = "DoActionOnRelease"))
	ELinkedButtonAction ReleasedAction {ELinkedButtonAction::Release};

	/** When true, the linked actor will not perform its gameplay action when triggered by this button. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Linked Button", Meta = (DisplayName = "Safe Link"))
	bool IsActionLinked {true};
};

/** Base class for button actors. */
UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "Interaction", Meta = (DisplayName = "Button"))
class APressableButton : public AActor, public IUsableObject
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogButton, Log, All)

protected:
	/** The root component for the actor. */
	UPROPERTY(BlueprintReadOnly, Category = "Button|Components", Meta = (DisplayName = "Root"))
	USceneComponent* RootSceneComponent;
	
	/** The base static mesh for the button. */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Button|Components", Meta = (DisplayName = "Base Mesh"))
	UStaticMeshComponent* BaseMesh;

	/** The button static mesh for the button. */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Button|Components", Meta = (DisplayName = "Button Mesh"))
	UStaticMeshComponent* ButtonMesh;
	
	// GENERAL
	/** The trigger type of the button. */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Button", Meta = (DisplayName = "Type"))
	EButtonTriggerType TriggerType {EButtonTriggerType::SinglePress};
	
	/** The cooldown time between presses. When the button is in cooldown, the button cannot be pressed. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Button", Meta = (DisplayName = "Cooldown Time", Units = "Seconds"))
	float CooldownTime {1.0f};

	/** When true, the button is currently pressed. This does not mean that the player is currently interacting with the button,
	 *	just that the button is in it's 'pressed-down' state. */
	UPROPERTY(BlueprintReadWrite, Category = "Button", Meta = (DisplayName = "Is Pressed"))
	bool IsPressed {false};

	/** Array of soft object pointers to triggerable actor instances. This allows us to affect other actors when this button is pressed. */
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "Button|Linked Actors", Meta = (DisplayName = "Target Actors",
		NoElementDuplicate, ShowOnlyInnerProperties))
	TArray<FButtonTargetActor> TargetActors;
	
	/** Array of soft object pointers to other button instances. This allows us to affect other button instances when this button is pressed. */
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "Button|Linked Actors", Meta = (DisplayName = "Linked Buttons",
		NoElementDuplicate, ShowOnlyInnerProperties))
	TArray<FLinkedButton> LinkedButtons;

	// PHYSICS
	/** If true, the button can be triggered by colliding physics objects. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Button|Collision", Meta = (DisplayName = "Can Be Triggered By Collisions"))
	bool CanTriggerByCollision {true};

	/** The force threshold required to trigger the button. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Button|Collision", Meta = (DisplayName = "Collision Trigger Threshold", Units = "Newtons",
		EditCondition = "CanTriggerByCollision"))
	float CollisionTriggerThreshold {100.0f};
	
	/** The mesh collision trigger component. */
	UPROPERTY(BlueprintReadOnly, Category = "Button|Components", Meta = (DisplayName = "Collision Trigger Component"))
	UMeshCollisionTriggerComponent* CollisionTriggerComponent;
	
	// POWER
	/** If true, the button requires power to operate and can be connected to a power source. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Button|Power", Meta = (DisplayName = "Requires Power"))
	bool RequiresPower {false};

	/** Soft object pointer to the power source that this button is connected to. */
	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category = "Button|Power", Meta = (DisplayName = "Power Source", EditCondition = "RequiresPower"))
	TSoftObjectPtr<APowerSource> PowerSource;

	/** If true, the button can still be toggled when the button has no power.
	 *	This will update the state, but will not trigger any gameplay actions or call the linked buttons. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Button|Power", Meta = (DisplayName = "Can Be Toggled Without Power",
		EditCondition = "RequiresPower && TriggerType == EButtonTriggerType::Toggle", EditConditionHides))
	bool CanBeToggledWithoutPower {false};
	
	/** Defines the action the button should perform when power is lost. This is only available for toggle buttons. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Button|Power", Meta = (DisplayName = "Action On Power Loss",
		EditCondition = "RequiresPower && TriggerType == EButtonTriggerType::Toggle", EditConditionHides, ValidEnumValues = "Nothing, Release"))
	EButtonPowerChangeActionType PowerLossAction {EButtonPowerChangeActionType::Nothing};

	/** Defines the action the button should perform when power is regained. This is only available for toggle buttons.  */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Button|Power", Meta = (DisplayName = "Action On Power Gain",
		EditCondition = "RequiresPower && TriggerType == EButtonTriggerType::Toggle", EditConditionHides, ValidEnumValues = "Nothing, Press"))
	EButtonPowerChangeActionType PowerGainAction {EButtonPowerChangeActionType::Nothing};

	/** If true, the button is currently powered. */
	UPROPERTY(BlueprintReadOnly, Category = "Button|Power", Meta = (DisplayName = "Is Powered"))
	bool IsPowered {true};

	/** The power consumer component. */
	UPROPERTY(BlueprintReadOnly, Category = "Button|Components", Meta = (DisplayName = "Power Consumer Component"))
	UPowerConsumerComponent* PowerConsumerComponent;

private:
	/** If true, the button is currently in cooldown and cannot be pressed. */
	UPROPERTY(BlueprintGetter = GetIsCooldownActive, Category = "Button", Meta = (DisplayName = "Is Cooldown Active", EditCondition = "RequiresPower"))
	bool IsCooldownActive {false};
	
	/** Timer handle for the cooldown timer. */
	UPROPERTY()
	FTimerHandle CooldownTimerHandle;

public:	
	APressableButton();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
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

#if WITH_EDITOR
	/** Validates the array of target actors. */
	void ValidateTargetActors();

	/** Validates the array of linked buttons. */
	void ValidateLinkedButtons();
#endif
	
public:
	/** Presses the button. This function should not be called by the player: Implement the IInteractableObject interface instead.
	 *	This functions should only be called by linked objects.
	 *	@Param CallTargetActorss If true, the event should execute its gameplay actions.
	 *	@Param CallLinkedButtons If true, the button should perform the specified actions on other buttons.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Button|Events", Meta = (DisplayName = "Press Button"))
	void EventOnPress(const bool CallTargetActors = true, const bool CallLinkedButtons = true);

	/** Releases the button. This function should not be called by the player: Implement the IInteractableObject interface instead.
	 *	This functions should only be called by linked objects.
	 *	@Param CallTargetActors If true, the event should execute its gameplay actions.
	 *	@Param CallLinkedButtons If true, the button should perform the specified actions on other buttons.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Button|Events", Meta = (DisplayName = "Release Button"))
	void EventOnRelease(const bool CallTargetActors = true, const bool CallLinkedButtons = true);

	/** Event called when the button is triggered by a collision. */
	UFUNCTION(BlueprintNativeEvent, Category = "Button|Events", Meta = (DisplayName = "On Collision Trigger"))
	void EventOnCollisionTrigger();

	/** Event called when the button's power state changes. */
	UFUNCTION(BlueprintNativeEvent, Category = "Button|Events", Meta = (DisplayName = "On Power State Changed"))
	void EventOnPowerStateChanged(bool NewState);

	/** Returns whether the button is currently pressed. */
	UFUNCTION()
	FORCEINLINE bool GetIsPressed() const { return IsPressed; }
	
	/** Returns whether the button is currently in cooldown. */
	UFUNCTION(BlueprintGetter, Category = "Button", Meta = (DisplayName = "Is Cooldown Active"))
	FORCEINLINE bool GetIsCooldownActive() const { return IsCooldownActive; }
};
