// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHDragComponent.h"

bool UBHDragComponent::BeginDrag_Implementation(const AActor* Interactor)
{
	HandleOnInteractionStart();
	return true;
}

bool UBHDragComponent::EndDrag_Implementation(const AActor* Interactor)
{
	HandleOnInteractionEnd();
	return true;
}
