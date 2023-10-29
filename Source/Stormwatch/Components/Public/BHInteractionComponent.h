// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "Components/SceneComponent.h"
#include "BHInteractableObjectInterface.h"
#include "BHInteractionComponent.generated.h"

UCLASS(Abstract, NotBlueprintable, BlueprintType, ClassGroup = "Interaction")
class UBHInteractionComponent : public USceneComponent, public IBHInteractableObject  
{
	GENERATED_BODY()
	
public:
	//~ Begin BHInteractableObject interface.
	FVector GetInteractionWidgetLocation_Implementation() const override { return GetComponentLocation(); }
	//~ End BHInteractableObject interface.

	bool IsInteracting() const { return bInteracting; }

protected:
	void OnRegister() override;

	bool bInteracting = false;
#if WITH_EDITOR
	bool bAllowOnlyOneInstancePerActor = false;
#endif
};
