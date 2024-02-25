// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "Stormwatch/Actors/Public/BHGrabbableMeshActor.h"

#include "BHGrabComponent.h"

ABHGrabbableMeshActor::ABHGrabbableMeshActor()
{
	GrabComponent = CreateDefaultSubobject<UBHGrabComponent>(TEXT("GrabComponent"));
	GrabComponent->SetupAttachment(GetStaticMeshComponent());
}

bool ABHGrabbableMeshActor::AddToInventory_Implementation()
{
	return false;
}

bool ABHGrabbableMeshActor::RemoveFromInventory_Implementation()
{
	return false;
}

bool ABHGrabbableMeshActor::CanAddToInventory_Implementation() const
{
	return bCanBeAddedToInventory;
}

void ABHGrabbableMeshActor::BeginPlay()
{
	Super::BeginPlay();
}

UBHGrabComponent* ABHGrabbableMeshActor::GetGrabComponent() const
{
	return GrabComponent;
}



