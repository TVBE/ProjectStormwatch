// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#include "Mouse.h"

#include "MeshInteractionComponent.h"


AMouse::AMouse()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	MouseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = MouseMesh;

	InteractionComponent = CreateDefaultSubobject<UMeshInteractionComponent>("Interaction Component");
}

void AMouse::PostInitProperties()
{
	Super::PostInitProperties();
	if (MouseMesh)
	{
		MouseMesh->BodyInstance.bGenerateWakeEvents = true;
	}
}

void AMouse::BeginPlay()
{
	Super::BeginPlay();
	MouseMesh->OnComponentWake.AddDynamic(this, &AMouse::HandleOnMeshWake);
	MouseMesh->OnComponentSleep.AddDynamic(this, &AMouse::HandleOnMeshSleep);
}

void AMouse::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMouse::HandleOnMeshWake(UPrimitiveComponent* WakingComponent, FName Name)
{
	this->SetActorTickEnabled(true);
}

void AMouse::HandleOnMeshSleep(UPrimitiveComponent* SleepingComponent, FName Name)
{
	this->SetActorTickEnabled(false);
}





