// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "Stormwatch/Actors/Public/BHGrabbableMeshActor.h"

#include "BHGrabbableComponent.h"

ABHGrabbableMeshActor::ABHGrabbableMeshActor()
{
	GrabbableComponent = CreateDefaultSubobject<UBHGrabbableComponent>(TEXT("GrabbableComponent"));
	if(GrabbableComponent)
	{
		GrabbableComponent->SetupAttachment(GetStaticMeshComponent());
	}
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

UBHGrabbableComponent* ABHGrabbableMeshActor::GetGrabComponent() const
{
	return GrabbableComponent;
}



