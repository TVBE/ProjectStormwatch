// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "InteractableObjectInterface.generated.h"

/** Interface for grabbable objects. This interface can be implemented in any UObject derived class. */
UINTERFACE(Blueprintable, Meta = (DisplayName = "Interactable Object Interface", ShortToolTip = "Interface for interactable objects."))
class FROSTBITE_API UInteractableObject : public UInterface
{
	GENERATED_BODY()
};

class FROSTBITE_API IInteractableObject
{
	GENERATED_BODY()

public:
	/** Returns the interaction widget location in world space. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable Object", Meta = (Displayname = "Get Interaction Widget Location"))
	FVector GetInteractionWidgetLocation() const;
};