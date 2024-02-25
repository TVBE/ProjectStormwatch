// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "Stormwatch/Actors/Public/BHDraggableMeshActor.h"

#include "BHDragComponent.h"

ABHDraggableMeshActor::ABHDraggableMeshActor()
{
	DragComponent = CreateDefaultSubobject<UBHDragComponent>(TEXT("DragComponent"));
	DragComponent->SetupAttachment(GetStaticMeshComponent());
}

UBHDragComponent* ABHDraggableMeshActor::GetDragComponent() const
{
	return DragComponent;
}

void ABHDraggableMeshActor::BeginPlay()
{
	Super::BeginPlay();
}

