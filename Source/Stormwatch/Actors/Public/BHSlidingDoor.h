// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "TriggerableObjectInterface.h"
#include "GameFramework/Actor.h"
#include "BHSlidingDoor.generated.h"

class UPowerConsumerComponent;
class ABHPowerSource;

UENUM(BlueprintType)
enum class EBHDoorState : uint8
{
	Open        UMETA(DisplayName = "Open"), 
	Closed      UMETA(DisplayName = "Closed"),
	Opening     UMETA(DisplayName = "Opening"),
	Closing     UMETA(DisplayName = "Closing"),
};

UENUM(BlueprintType)
enum class EBHDoorAction : uint8
{
	Open        UMETA(DisplayName = "Open"), 
	Close		UMETA(DisplayName = "Close"),
	Reset		UMETA(Displayname = "Reset")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDoorStateChangedDelegate, EBHDoorState, State);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCooldownFinishedDelegate);

/** Abstract base class for sliding door actors. */
UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "Interaction", Meta = (DisplayName = "Sliding Door", PrioritizeCategories = "Door"))
class ABHSlidingDoor : public AActor
{
	GENERATED_BODY()

public:
	/** Delegate that is called when the door is opening. */
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnDoorStateChangedDelegate OnDoorStateChanged;
	
	/** Delegate that is called when the door is no longer in cooldown. */
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnCooldownFinishedDelegate OnCooldownFinished;

protected:
	/** The box component that is used to determine if an object or pawn is inside the door.
	 *	This is used to prevent the door from crushing actors. */
	UPROPERTY(BlueprintReadOnly, Category = "Components")
	class UBoxComponent* SafetyBox;

	/** The starting state of the door. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door", Meta = (ValidEnumValues = "Open, Closed"))
	EBHDoorState StartingState = EBHDoorState::Closed;

	/** The current state of the door. */
	UPROPERTY(BlueprintReadWrite)
	EBHDoorState DoorState;

	/** If true, the door will start locked. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door|Lock")
	bool StartLocked = false;

	/** If true, the door will close when it is locked. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door|Lock")
	bool CloseOnLock = true;

	/** If true, the door will either open or reset when unlocked. */
	UPROPERTY(EditAnywhere, Meta = (InlineEditConditionToggle))
	bool DoActionOnUnlock = false;

	/** The action to perform when the door is unlocked. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door|Lock", Meta = (EditCondition = "DoActionOnUnlock"))
	EBHDoorAction ActionOnUnlock = EBHDoorAction::Reset;

	EBHDoorState StateWhenLocked;
	
	/** If true, the door will auto close after a given time when opened. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door|Auto Close", Meta = (DisplayName = "Auto Close (Not Available)")) // TODO: Implement this.
	bool IsAutoCloseEnabled = false;

	/** The time required for the door to close automatically after the last trigger. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door|Auto Close", Meta = (DisplayName = "Auto Close Delay (Not Available)",
		Units = "Seconds", ClampMin = "0")) // TODO: Implement this.
	float AutoCloseDelay = 3.0f;

	/** If true, the door will open if a pawn enters it's safety zone while the door is closing. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (DisplayName = "Cancel Close If Pawn Enters Safety Zone"))
	bool CancelCloseOnSafetyZoneOverlap = false;
	
	/** If true, the button requires power to operate and can be connected to a power source. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door|Power")
	bool RequiresPower = false;
	
	/** Soft object pointer to the power source that this button is connected to. */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Door|Power", Meta = (EditCondition = "RequiresPower"))
	TSoftObjectPtr<ABHPowerSource> PowerSource;
	
	/** If true, the door should perform an action when power is regained. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (InlineEditConditionToggle))
	bool DoActionOnPowerGain = false;

	/** Action the door should perform when power is regained. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door|Power", Meta = (EditCondition = "DoActionOnPowerGain"))
	EBHDoorAction ActionOnPowerGain = EBHDoorAction::Open;

	/** If true, the door should perform an action when power is lost. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (InlineEditConditionToggle))
	bool DoActionOnPowerLoss = false;

	/** Action the door should perform when power is lost. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door|Power", Meta = (EditCondition = "DoActionOnPowerLoss"))
	EBHDoorAction ActionOnPowerLoss = EBHDoorAction::Open;

	/** The power consumer component. */
	UPROPERTY(BlueprintReadOnly, Category = "Components")
	UPowerConsumerComponent* PowerConsumerComponent;

private:
	/** The root component for the actor. */
	UPROPERTY(BlueprintReadOnly, Meta = (DisplayName = "Root", AllowPrivateAccess = "true"))
	USceneComponent* RootSceneComponent;
	
	/** Queue of ITriggerableObject function calls made by other objects. */
	UPROPERTY(BlueprintGetter = GetTriggerQueue)
	TArray<ETriggerableObjectAction> TriggerQueue;

	/** If true, the door is currently locked and cannot be opened or closed. */
	UPROPERTY(BlueprintGetter = GetIsLocked)
	bool IsLocked = false;

	/** If true, the door is currently in a cooldown state, and cannot close or open for the remainder of the cooldown duration. */
	UPROPERTY(BlueprintGetter = GetIsCooldownActive)
	bool IsCooldownActive = false;

	/** If true, the door will ignore triggers while in cooldown mode.
	 *	By default, triggers are accumulated in a queue and evaluated at the end of the cooldown period. */
	UPROPERTY(EditAnywhere, Category = "Cooldown", Meta = (DisplayAfter = "CooldownTime"))
	bool IgnoreTriggersDuringCooldown = false;

	/** The cooldown time for the door before it can open or close again after being triggered. */
	UPROPERTY(EditAnywhere, Category = "Cooldown", Meta = (DisplayName = "Cooldown", Units = "Seconds",
		ClampMin = "0", ClampMax = "10", UIMin = "0", UIMax = "10"))
	float CooldownTime = 1.0f;

	/** If true, the actor currently wants to set the safety zone collision box to block pawns,
	 *	but there is a pawn inside the box preventing the actor from doing so. */
	bool WantsToSetSafetyZoneToBlocking = false;
	
	
	/** Timer handle for the cooldown timer. */
	UPROPERTY()
	FTimerHandle CooldownTimerHandle;

	/** Timer handle for the close safety check timer. */
	UPROPERTY()
	FTimerHandle CloseCheckTimerHandle;

public:	
	ABHSlidingDoor();
	
	/** Checks whether the door can currently close. If a pawn is inside the safety box, this is not allowed unless overridden. */
	UFUNCTION(BlueprintPure)
	bool CanClose() const;

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable)
	void StartCooldown();

	/** Sets the collision profile of the safety zone. If we enable the safety zone collision while a pawn is inside,
	 *	the pawn will be pushed out gradually out of the box before full blocking collision is enabled. */
	UFUNCTION(BlueprintCallable)
	void SetSafetyZoneCollisionEnabled(const bool Value);

private:
	void Tick(float DeltaSeconds) override;
	
	void HandleCooldownFinished();

	/** Tries to close the door if possible. This function is set on a timer if the door wishes to close but is prevented
	 *	from doing so because a pawn is overlapping the safety zone.*/
	UFUNCTION()
	void HandleCloseTimerUpdate();

	/** Pushes any overlapping pawn out of the door safety box. */
	UFUNCTION()
	void PushActorsOutOfSafetyBox(TArray<AActor*> Actors, const float DeltaTime);

public:
	/** Attempts to open the door. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Meta = (DisplayName = "Open", Keywords = "Open"))
	void Open();

	/** Attempts to close the door. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Meta = (DisplayName = "Close", Keywords = "Close"))
	void Close();
	
	/** Locks the door. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Meta = (DisplayName = "Lock", Keywords = "Lock"))
	void Lock();
	
	/** Unlocks the door. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Meta = (DisplayName = "Unlock", Keywords = "Unlock"))
	void Unlock();

protected:
	/** Event called when the door has opened. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Events", Meta = (DisplayName = "Opened"))
	void EventOpened();
	
	/** Event called when the door has closed. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Events", Meta = (DisplayName = "Closed"))
	void EventClosed();

	/** Event called when the power state of the door has changed. */
	UFUNCTION(BlueprintNativeEvent, Category = "Events", Meta = (DisplayName = "On Power State Changed"))
	void EventOnPowerStateChanged(const bool NewState);

public:
	/** Returns the trigger queue of the door. */
	UFUNCTION(BlueprintGetter)
	FORCEINLINE TArray<ETriggerableObjectAction> GetTriggerQueue() const { return TriggerQueue; }

	/** Returns whether the door is locked or not. */
	UFUNCTION(BlueprintGetter)
	FORCEINLINE bool GetIsLocked() const { return IsLocked; }

	/** Returns whether the door is in cooldown or not. */
	UFUNCTION(BlueprintGetter)
	FORCEINLINE bool GetIsCooldownActive() const { return IsCooldownActive; }

	/** Returns the door's state. */
	UFUNCTION(BlueprintPure)
	FORCEINLINE EBHDoorState GetDoorState() const { return DoorState; }
};


