// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BHTriggerableObjectInterface.generated.h"

UENUM(BlueprintType)
enum class ETriggerableObjectAction : uint8
{
	Trigger			UMETA(DisplayName = "Trigger"),
	Untrigger		UMETA(DisplayName = "Untrigger"),
};

UINTERFACE(Blueprintable)
class UBHTriggerableObject : public UInterface
{
	GENERATED_BODY()
};

class IBHTriggerableObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Triggerable Object")
	bool Trigger(const AActor* Initiator);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Triggerable Object")
	bool Untrigger(const AActor* Initiator);
};

