// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "InteractableObjectInterface.generated.h"

UINTERFACE(NotBlueprintable)
class FROSTBITE_API UInteractableObject : public UInterface
{
	GENERATED_BODY()
};

class FROSTBITE_API IInteractableObject
{
	GENERATED_BODY()

public:
	/** Checks if the object can be interacted with. */
	UFUNCTION(BlueprintCallable, Category = "InteractableObject", Meta = (DisplayName = "Can Interact"))
	virtual bool CanInteract() const = 0;

	/** Begins interaction with the object. */
	UFUNCTION(BlueprintCallable, Category = "InteractableObject", Meta = (DisplayName = "Begin Interaction"))
	virtual void BeginInteraction() = 0;

	/** Ends interaction with the object. */
	UFUNCTION(BlueprintCallable, Category = "InteractableObject", Meta = (DisplayName = "End Interaction"))
	virtual void EndInteraction() = 0;
};