// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BHDraggableObjectInterface.generated.h"

UINTERFACE(Blueprintable)
class STORMWATCH_API UBHDraggableObject : public UInterface
{
	GENERATED_BODY()
};

class STORMWATCH_API IBHDraggableObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BHDraggableObject")
	bool Drag(const AActor* Instigator);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BHDraggableObject")
	void Release(const AActor* Instigator);
};