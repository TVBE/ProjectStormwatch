// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "TriggerableObjectInterface.h"
#include "GameFramework/Actor.h"
#include "PowerSource.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPowerStateChangedDelegate, bool, NewState);

/** Base class for power source actors. */
UCLASS(Blueprintable, BlueprintType, ClassGroup = "Power", Meta = (DisplayName = "Power Source"))
class APowerSource : public AActor, public ITriggerableObject
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogPowerSource, Log, All)

public:
	/** Delegate that is called when the power state of this power source changes. */
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Power Source|Delegates", Meta = (DisplayName = "On Power State Changed"))
	FOnPowerStateChangedDelegate OnPowerStateChanged;

protected:
	/** If true, this power source is currently active and providing power to its registered power consumers. */
	UPROPERTY(BlueprintReadOnly, Category = "Power Source", Meta = (DisplayName = "Is Energized"))
	bool IsEnergized {true};

private:
	/** Array of power consumers that are registered to this power source. */
	UPROPERTY(BlueprintGetter = GetConnectedConsumers, Category = "Power Source", Meta = (DisplayName = "Connected Objects"))
	TArray<UObject*> ConnectedConsumers;

public:	
	APowerSource();

	bool Trigger_Implementation(const AActor* Initiator) override;
	bool Untrigger_Implementation(const AActor* Initiator) override;

	/** Registers a power consumer to this power source. */
	UFUNCTION(BlueprintCallable, Category = "Power Source", Meta = (DisplayName = "Register Power Consumer"))
	void RegisterPowerConsumer(UObject* Consumer);

protected:
	virtual void BeginPlay() override;

	/** Energizes or de-energizes the power source. */
	UFUNCTION(BlueprintCallable, Category = "Power Source", Meta = (DisplayName = "Set Power Source State"))
	void SetPowerSourceState(const bool State);

public:
	/** Returns the power consumer objects that are connected to this power source. */
	UFUNCTION(BlueprintPure, Category = "Power Source", Meta = (DisplayName = "Connected Objects"))
	FORCEINLINE TArray<UObject*> GetConnectedConsumers() const { return ConnectedConsumers; }
};
