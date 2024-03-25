// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHDraggableComponent.h"

bool UBHDraggableComponent::Drag_Implementation(const AActor* Instigator)
{
	BeginInteraction();
	return true;
}

void UBHDraggableComponent::Release_Implementation(const AActor* Instigator)
{
	EndInteraction();
	return;
}
