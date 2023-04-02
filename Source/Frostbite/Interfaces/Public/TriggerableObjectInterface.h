// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TriggerableObjectInterface.generated.h"

UENUM(BlueprintType)
enum class ETriggerableObjectAction : uint8
{
	Trigger			UMETA(DisplayName = "Trigger"),
	Untrigger		UMETA(DisplayName = "Untrigger"),
};

UINTERFACE(Blueprintable, Meta = (DisplayName = "Triggerable Object Interface"))
class UTriggerableObject : public UInterface
{
	GENERATED_BODY()
};

class ITriggerableObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "TriggerableObject", Meta = (DisplayName = "Trigger"))
	bool Trigger(const AActor* Initiator);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "TriggerableObject", Meta = (DisplayName = "Untrigger"))
	bool Untrigger(const AActor* Initiator);
};