// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "InteractableObjectInterface.h"
#include "MeshInteractionComponent.generated.h"

UCLASS(Abstract, NotBlueprintable, BlueprintType, ClassGroup = "Interaction",
	Meta = (DisplayName = "Mesh Interaction Component", ShortToolTip = "Base class for component that adds interactability to static mesh actors."))
class UMeshInteractionComponent : public USceneComponent, public IInteractableObject  
{
	GENERATED_BODY()
	
public:
	/** IInteractableObject interface functions. */
	FORCEINLINE FVector GetInteractionWidgetLocation_Implementation() const override { return GetComponentLocation(); }
	
protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
};
