// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "BHPhysicsInteractionComponent.h"
#include "BHDraggableObjectInterface.h"
#include "BHDragComponent.generated.h"

UCLASS(NotBlueprintable, BlueprintType, ClassGroup = "Interaction", Meta = (BlueprintSpawnableComponent))
class UBHDragComponent : public UBHPhysicsInteractionComponent, public IBHDraggableObject 
{
	GENERATED_BODY()

public:
	//~ Begin BHDraggableObject Interface.
	bool BeginDrag_Implementation(const AActor* Interactor) override;
	bool EndDrag_Implementation(const AActor* Interactor) override;
	//~ End BHDraggableObject Interface.
};
