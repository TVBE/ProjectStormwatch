// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite


#include "MeshDragComponent.h"

bool UMeshDragComponent::BeginDrag_Implementation(const AActor* Interactor)
{
	return IDraggableObject::BeginDrag_Implementation(Interactor);
}

bool UMeshDragComponent::EndDrag_Implementation(const AActor* Interactor)
{
	return IDraggableObject::EndDrag_Implementation(Interactor);
}
