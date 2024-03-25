// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "BHPhysicsInteractableComponent.h"
#include "BHDraggableObjectInterface.h"
#include "BHDraggableComponent.generated.h"

UCLASS(Blueprintable, BlueprintType, ClassGroup = "Barrelhouse", Meta = (BlueprintSpawnableComponent))
class UBHDraggableComponent : public UBHPhysicsInteractableComponent, public IBHDraggableObject 
{
	GENERATED_BODY()

public:
	//~ Begin BHDraggableObject Interface.
	virtual bool Drag_Implementation(const AActor* Instigator) override;
	virtual void Release_Implementation(const AActor* Instigator) override;
	//~ End BHDraggableObject Interface.
};
