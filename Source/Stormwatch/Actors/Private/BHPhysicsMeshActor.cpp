// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "Stormwatch/Actors/Public/BHPhysicsMeshActor.h"

#include "ReacousticComponent.h"

ABHPhysicsMeshActor::ABHPhysicsMeshActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = StaticMeshComponent;
	StaticMeshComponent->Mobility = EComponentMobility::Movable;

	ReacousticComponent = CreateDefaultSubobject<UReacousticComponent>(TEXT("ReacousticComponent"));
}

void ABHPhysicsMeshActor::BeginPlay()
{
	Super::BeginPlay();
}

UStaticMeshComponent* ABHPhysicsMeshActor::GetStaticMeshComponent() const
{
	return StaticMeshComponent;
}

UReacousticComponent* ABHPhysicsMeshActor::GetReacousticComponent() const
{
	return ReacousticComponent;
}

