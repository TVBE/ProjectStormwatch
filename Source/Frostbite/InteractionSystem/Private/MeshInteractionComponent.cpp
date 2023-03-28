// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#include "MeshInteractionComponent.h"
#include "CollisionQueryParams.h"

void UMeshInteractionComponent::OnComponentCreated()
{
	Super::OnComponentCreated();

	if(IsAutoConfigurable)
	{
		
	}
	else
	{
		InteractionType = IsHeavy ? EInteractionType::Draggable : EInteractionType::Grabbable;
		InteractionHandType = IsLarge ? EInteractionHandType::TwoHanded : EInteractionHandType::OneHanded;
	}
}
