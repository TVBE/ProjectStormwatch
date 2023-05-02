// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PowerConsumerInterface.generated.h"

UINTERFACE(Blueprintable, Meta = (DisplayName = "Power Consumer Interface",
	ShortToolTip = "Interface for objects that consume power."))
class UPowerConsumer : public UInterface
{
	GENERATED_BODY()
};

class IPowerConsumer
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Power Consumer")
	bool SetPowerState(const bool NewPowerState, const AActor* Initiator);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Power Consumer")
	bool GetPowerState() const;
};