// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BHInteractableObjectInterface.generated.h"

UINTERFACE(Meta = (DisplayName = "Interactable Object Interface", ShortToolTip = "Interface for interactable objects."))
class STORMWATCH_API UBHInteractableObject : public UInterface
{
	GENERATED_BODY()
};

class STORMWATCH_API IBHInteractableObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BHInteractableObject")
	bool CanBeInteractedWith() const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BHInteractableObject")
	FVector GetInteractionLocation() const;
};