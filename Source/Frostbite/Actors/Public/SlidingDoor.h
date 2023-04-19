// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "DoorInterface.h"
#include "TriggerableObjectInterface.h"
#include "GameFramework/Actor.h"
#include "SlidingDoor.generated.h"

class UPowerConsumerComponent;
class APowerSource;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDoorStateChangedDelegate, EDoorState, State);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCooldownFinishedDelegate);

/** Abstract base class for sliding door actors. */
UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "Interaction", Meta = (DisplayName = "Sliding Door"))
class ASlidingDoor : public AActor, public IDoor, public ITriggerableObject
{
	GENERATED_BODY()

public:
	/** Delegate that is called when the door is opening. */
	UPROPERTY(BlueprintAssignable, Category = "Door|Delegates", Meta = (DisplayName = "On Door State Changed"))
	FOnDoorStateChangedDelegate OnDoorStateChanged;
	
	/** Delegate that is called when the door is no longer in cooldown. */
	UPROPERTY(BlueprintAssignable, Category = "Door|Delegates", Meta = (DisplayName = "On Cooldown Finished"))
	FOnCooldownFinishedDelegate OnCooldownFinished;

protected:
	/** The box component that is used to determine if an object or pawn is inside the door.
	 *	This is used to prevent the door from crushing actors. */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Door", Meta = (DisplayName = "Safety Box"))
	class UBoxComponent* SafetyBox;

	/** The starting state of the door. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Door", Meta = (DisplayName = "Starting State",
		ValidEnumValues = "Open, Closed"))
	EDoorState StartingState {EDoorState::Closed};

	/** The current state of the door. */
	UPROPERTY(BlueprintReadWrite, Category = "Door", Meta = (DisplayName = "Door State"))
	EDoorState DoorState;

	/** If true, the door will auto close after a given time when opened. */
	UPROPERTY(BlueprintReadOnly, Category = "Door|Auto Close", Meta = (DisplayName = "Auto Close"))
	bool IsAutoCloseEnabled {false};

	/** The time required for the door to close automatically after the last trigger. */
	UPROPERTY(BlueprintReadOnly, Category = "Door|Auto Close", Meta = (DisplayName = "Auto Close Delay",
		Units = "Seconds", ClampMin = "0"))
	float AutoCloseDelay {3.0f};

	/** If true, the button requires power to operate and can be connected to a power source. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Door|Power", Meta = (DisplayName = "Requires Power"))
	bool RequiresPower {false};
	
	/** Soft object pointer to the power source that this button is connected to. */
	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category = "Door|Power", Meta = (DisplayName = "Power Source",
		EditCondition = "RequiresPower"))
	TSoftObjectPtr<APowerSource> PowerSource;

	/** If true, the door should reset to its state before a power loss. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Door|Power", Meta = (DisplayName = "Reset State On Power Gain"))
	bool ResetStateOnPowerGain {false};

	/** If true, the door should perform an action when power is regained. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Door|Power", Meta = (DisplayName = "Do Action On Power Gain",
		EditCondition = "RequiresPower && !ResetStateOnPowerGain"))
	bool DoActionOnPowerGain {false};

	/** Action the door should perform when power is regained. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Door|Power", Meta = (DisplayName = "Action On Power Gain",
		EditCondition = "DoActionOnPowerGain && RequiresPower && !ResetStateOnPowerGain"))
	EDoorAction ActionOnPowergain {EDoorAction::Open};

	/** If true, the door should perform an action when power is lost. */
	UPROPERTY(BlueprintReadOnly, Category = "Door|Power", Meta = (DisplayName = "Do Action On Power Loss", InlineEditConditionToggle))
	bool DoActionOnPowerLoss {false};

	/** Action the door should perform when power is lost. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Door|Power", Meta = (DisplayName = "Action On Power Loss",
		EditCondition = "DoActionOnPowerLoss"))
	EDoorAction ActionOnPowerLoss {EDoorAction::Open};

	/** The power consumer component. */
	UPROPERTY(BlueprintReadOnly, Category = "Door|Components", Meta = (DisplayName = "Power Consumer Component"))
	UPowerConsumerComponent* PowerConsumerComponent;

private:
	/** Queue of ITriggerableObject function calls made by other objects. */
	UPROPERTY(BlueprintGetter = GetTriggerQueue, Category = "Door", Meta = (DisplayName = "Trigger Queue"))
	TArray<ETriggerableObjectAction> TriggerQueue;

	/** If true, the door is currently locked and cannot be opened or closed. */
	UPROPERTY(BlueprintGetter = GetIsLocked, EditInstanceOnly, Category = "Door", Meta = (DisplayName = "Is Locked"))
	bool IsLocked {false};

	/** If true, the door is currently in a cooldown state, and cannot close or open for the remainder of the cooldown duration. */
	UPROPERTY(BlueprintGetter = GetIsCooldownActive, Category = "Door|Cooldown", Meta = (DisplayName = "Is Cooldown Active"))
	bool IsCooldownActive {false};

	/** If true, the door will ignore triggers while in cooldown mode.
	 *	By default, triggers are accumulated in a queue and evaluated at the end of the cooldown period. */
	UPROPERTY(EditAnywhere, Category = "Door|Cooldown", Meta = (DisplayName = "Ignore Triggers During Cooldown", DisplayAfter = "CooldownTime"))
	bool IgnoreTriggersDuringCooldown {false};

	/** The cooldown time for the door before it can open or close again after being triggered. */
	UPROPERTY(EditAnywhere, Category = "Door|Cooldown", Meta = (DisplayName = "Cooldown Time", ClampMin = "0", Units = "Seconds"))
	float CooldownTime {1.0f};

	/** Timer handle for the cooldown timer. */
	UPROPERTY()
	FTimerHandle CooldownTimerHandle;

public:	
	ASlidingDoor();
	virtual void Tick(float DeltaTime) override;

	bool Open_Implementation(const AActor* Initiator) override;
	bool Close_Implementation(const AActor* Initiator) override;
	FORCEINLINE EDoorState GetDoorState_Implementation() const override { return DoorState; }

	bool Trigger_Implementation(const AActor* Initiator) override;
	bool Untrigger_Implementation(const AActor* Initiator) override;

protected:
	virtual void BeginPlay() override;

	/** begins the cooldown timer for the door. */
	UFUNCTION(BlueprintCallable, Category = "Door", Meta = (DisplayName = "Start Cooldown"))
	void StartCooldown();

private:
	void AddToTriggerQueue(const ETriggerableObjectAction Value);

	ETriggerableObjectAction EvaluateAndClearTriggerQueue();

	/** Called when the cooldown timer is finished. */
	void HandleCooldownFinished();

protected:
	/** Event called when the door opens. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Door|Events", Meta = (DisplayName = "Open Door"))
	void EventDoorOpen();

	/** Event called when the door has opened. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Door|Events", Meta = (DisplayName = "Door Opened"))
	void EventDoorOpened();

	/** Event called when the door closes. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Door|Events", Meta = (DisplayName = "Close Door"))
	void EventDoorClose();

	/** Event called when the door has closed. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Door|Events", Meta = (DisplayName = "Door Closed"))
	void EventDoorClosed();

	/** Event called when the power state of the door has changed. */
	UFUNCTION(BlueprintNativeEvent, Category = "Door|Events", Meta = (DisplayName = "On Power State Changed"))
	void EventOnPowerStateChanged(const bool NewState);

public:
	/** Returns the trigger queue of the door. */
	UFUNCTION(BlueprintGetter, Category = "Door", Meta = (DisplayName = "Trigger Queue"))
	FORCEINLINE TArray<ETriggerableObjectAction> GetTriggerQueue() const { return TriggerQueue; }

	/** Returns whether the door is locked or not. */
	UFUNCTION(BlueprintGetter, Category = "Door", Meta = (DisplayName = "Is Locked"))
	FORCEINLINE bool GetIsLocked() const { return IsLocked; }

	/** Returns whether the door is in cooldown or not. */
	UFUNCTION(BlueprintGetter, Category = "Door|Cooldown", Meta = (DisplayName = "Is Cooldown Active"))
	FORCEINLINE bool GetIsCooldownActive() const { return IsCooldownActive; }
};
