// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BHPowerConsumerInterface.generated.h"

UINTERFACE(Blueprintable)
class UBHPowerConsumer : public UInterface
{
	GENERATED_BODY()
};

class IBHPowerConsumer
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Power Consumer")
	bool SetPowerState(const bool NewPowerState, const AActor* Initiator);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Power Consumer")
	bool GetPowerState() const;
};