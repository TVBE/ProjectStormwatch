// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "BHPowerConsumerInterface.h"
#include "BHPowerSource.h"
#include "BHPowerConsumptionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPowerConsumerComponentStateChangedDelegate, bool, NewState);

UCLASS(NotBlueprintable, BlueprintType, ClassGroup = "Power", Meta = (BlueprintSpawnableComponent))
class UBHPowerConsumptionComponent : public UActorComponent, public IBHPowerConsumer
{
	GENERATED_BODY()

public:
	UBHPowerConsumptionComponent();

	//~ Begin BHPowerConsumer Interface.
	bool SetPowerState_Implementation(const bool NewPowerState, const AActor* Initiator) override;
	bool GetPowerState_Implementation() const override { return bPowered; }
	//~ End BHPowerConsumer Interface.
	
	UFUNCTION(BlueprintPure)
	bool GetIsPowered() const { return bPowered; }
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Delegates")
	FOnPowerConsumerComponentStateChangedDelegate OnPowerStateChanged;
	
	UPROPERTY(BlueprintReadOnly, Meta = (DisplayName = "Target Power Source"))
	TSoftObjectPtr<ABHPowerSource> PowerSource;

private:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintGetter = GetIsPowered)
	bool bPowered = false;
};
