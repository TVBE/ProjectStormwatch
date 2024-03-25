// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "Stormwatch/Actors/Public/BHDraggableMeshActor.h"

#include "BHDraggableComponent.h"

ABHDraggableMeshActor::ABHDraggableMeshActor()
{
	DragComponent = CreateDefaultSubobject<UBHDraggableComponent>(TEXT("DragComponent"));
	if(DragComponent)
	{
		DragComponent->SetupAttachment(GetStaticMeshComponent());
	}
}

UBHDraggableComponent* ABHDraggableMeshActor::GetDragComponent() const
{
	return DragComponent;
}

void ABHDraggableMeshActor::BeginPlay()
{
	Super::BeginPlay();
}

