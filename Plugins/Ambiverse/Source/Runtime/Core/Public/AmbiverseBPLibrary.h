// Copyright (c) 2022-present Tim Verberne
// This source code is part of the Adaptive Ambience System plugin

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "AmbiverseBPLibrary.generated.h"

UCLASS()
class UAmbiverseBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, Category = "Ambiverse", Meta = (DisplayName = "Add Ambiverse Layer", Keywords = "Ambiverse Add Layer",
		WorldContext = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static void AddAmbiverseLayer(UObject* WorldContextObject, UAmbiverseLayer* AmbiverseLayer);

	UFUNCTION(BlueprintCallable, Category = "Ambiverse", Meta = (DisplayName = "Pop Ambiverse Layer", Keywords = "Ambiverse Pop Layer",
		WorldContext = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static void PopAmbiverseLayer(UObject* WorldContextObject, UAmbiverseLayer* AmbiverseLayer);

	UFUNCTION(BlueprintCallable, Category = "Ambiverse", Meta = (DisplayName = "Set Ambiverse Parameter", Keywords = "Ambiverse Set Parameter",
		WorldContext = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static void SetAmbiverseParameter(UObject* WorldContextObject);
};
