// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "ActorFunctionCaller.generated.h"

USTRUCT(BlueprintType)
struct FActorFunctionCaller
{
	GENERATED_BODY()

	/** The target actor to call the function on. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Actor Function Caller", Meta = (DisplayName = "Target"))
	TSoftObjectPtr<AActor> Target;

	/** The function to call on the actor.*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Actor Function Caller", Meta = (DisplayName = "Function"))
	FName FunctionName;

	void CallFunction();
};