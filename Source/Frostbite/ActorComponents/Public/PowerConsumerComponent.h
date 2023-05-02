// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "PowerConsumerInterface.h"
#include "PowerSource.h"
#include "Components/ActorComponent.h"
#include "PowerConsumerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPowerConsumerComponentStateChangedDelegate, bool, NewState);

UCLASS(NotBlueprintable, BlueprintType, ClassGroup = "Power", Meta = (BlueprintSpawnableComponent,
	DisplayName = "Power Consumer Component", ShortToolTip = "Component that can be added to actors that require power."))
class UPowerConsumerComponent : public UActorComponent, public IPowerConsumer
{
	GENERATED_BODY()

public:
	/** Delegate that is called when the power state of this power consumer changes. */
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Delegates")
	FOnPowerConsumerComponentStateChangedDelegate OnPowerStateChanged;

	/** The target power source this power consumer is dependent on for power. */
	UPROPERTY(BlueprintReadOnly, Meta = (DisplayName = "Target Power Source"))
	TSoftObjectPtr<APowerSource> PowerSource;

private:
	/** If true, this object is powered. */
	UPROPERTY(EditAnywhere, BlueprintGetter = GetIsPowered)
	bool IsPowered {false};

public:	
	UPowerConsumerComponent();

	bool SetPowerState_Implementation(const bool NewPowerState, const AActor* Initiator) override;
	bool GetPowerState_Implementation() const override { return IsPowered; }

protected:
	virtual void BeginPlay() override;

public:
	/** Returns whether the component is powered or not. */
	UFUNCTION(BlueprintPure)
	FORCEINLINE bool GetIsPowered() const { return IsPowered; }
};
