// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "DoorInterface.h"
#include "TriggerableObjectInterface.h"
#include "GameFramework/Actor.h"
#include "SlidingDoor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDoorOpenDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDoorOpenedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDoorCloseDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDoorClosedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCooldownFinishedDelegate);

/** Abstract base class for sliding door actors. */
UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "Interaction", Meta = (DisplayName = "Sliding Door"))
class ASlidingDoor : public AActor, public IDoor, public ITriggerableObject
{
	GENERATED_BODY()

public:
	/** Delegate that is called when the door is opening. */
	UPROPERTY(BlueprintAssignable, Category = "Door|Delegates", Meta = (DisplayName = "On Opening"))
	FOnDoorOpenDelegate OnDoorOpen;

	/** Delegate that is called when the door is opened. */
	UPROPERTY(BlueprintAssignable, Category = "Door|Delegates", Meta = (DisplayName = "On Opened"))
	FOnDoorOpenedDelegate OnDoorOpened;

	/** Delegate that is called when the door is closing. */
	UPROPERTY(BlueprintAssignable, Category = "Door|Delegates", Meta = (DisplayName = "On Closing"))
	FOnDoorCloseDelegate OnDoorClose;

	/** Delegate that is called when the door is closed. */
	UPROPERTY(BlueprintAssignable, Category = "Door|Delegates", Meta = (DisplayName = "On Closed"))
	FOnDoorClosedDelegate OnDoorClosed;

	/** Delegate that is called when the door is no longer in cooldown. */
	UPROPERTY(BlueprintAssignable, Category = "Door|Delegates", Meta = (DisplayName = "On Cooldown Finished"))
	FOnCooldownFinishedDelegate OnCooldownFinished;

protected:
	/** The box component that is used to determine if an object or pawn is inside the door.
	 *	This is used to prevent the door from crushing actors. */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Door", Meta = (DisplayName = "Safety Box"))
	class UBoxComponent* SafetyBox;

	/** The starting state of the door. */
	UPROPERTY(BlueprintReadOnly, Category = "Door", Meta = (DisplayName = "Starting State",
		ValidEnumValues = "Open, Closed"))
	EDoorState StartingState {EDoorState::Closed};

	/** The current state of the door. */
	UPROPERTY(BlueprintReadWrite, Category = "Door", Meta = (DisplayName = "Door State"))
	EDoorState DoorState;

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

	void Open_Implementation() override;
	void Close_Implementation() override;
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
	UFUNCTION(BlueprintNativeEvent, Category = "Door|Events", Meta = (DisplayName = "Open Door"))
	void EventDoorOpen();

	/** Event called when the door has opened. */
	UFUNCTION(BlueprintNativeEvent, Category = "Door|Events", Meta = (DisplayName = "Handle Door Opened"))
	void EventDoorOpened();

	/** Event called when the door closes. */
	UFUNCTION(BlueprintNativeEvent, Category = "Door|Events", Meta = (DisplayName = "Open Door"))
	void EventDoorClose();

	/** Event called when the door has closed. */
	UFUNCTION(BlueprintNativeEvent, Category = "Door|Events", Meta = (DisplayName = "Handle Door Closed"))
	void EventDoorClosed();

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
