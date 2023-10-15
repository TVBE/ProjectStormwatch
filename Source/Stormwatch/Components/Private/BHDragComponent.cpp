// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHDragComponent.h"

bool UBHDragComponent::BeginDrag_Implementation(const AActor* Interactor)
{
	return IBHDraggableObject::BeginDrag_Implementation(Interactor);
}

bool UBHDragComponent::EndDrag_Implementation(const AActor* Interactor)
{
	return IBHDraggableObject::EndDrag_Implementation(Interactor);
}
