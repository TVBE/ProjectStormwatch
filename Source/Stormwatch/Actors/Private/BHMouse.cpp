// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHMouse.h"
#include "MeshGrabComponent.h"

ABHMouse::ABHMouse()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	MouseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = MouseMesh;

	GrabComponent = CreateDefaultSubobject<UMeshGrabComponent>("Interaction Component");
	GrabComponent->SetupAttachment(RootComponent);
}

void ABHMouse::PostInitProperties()
{
	Super::PostInitProperties();
	if (MouseMesh)
	{
		MouseMesh->BodyInstance.bGenerateWakeEvents = true;
	}
}

void ABHMouse::BeginPlay()
{
	Super::BeginPlay();
	MouseMesh->OnComponentWake.AddDynamic(this, &ABHMouse::HandleOnMeshWake);
	MouseMesh->OnComponentSleep.AddDynamic(this, &ABHMouse::HandleOnMeshSleep);
}

void ABHMouse::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	FVector Start = MouseMesh->GetComponentLocation();
	FVector End = Start + FVector(0, 0, -15);
	FHitResult HitResult;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = false;

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, QueryParams))
	{
		FVector Velocity = MouseMesh->GetComponentVelocity();
		FVector2D MouseVelocity(Velocity.X, Velocity.Y);
		OnMouseMovement.Broadcast(MouseVelocity);
	}
}

void ABHMouse::HandleOnMeshWake(UPrimitiveComponent* WakingComponent, FName Name)
{
	this->SetActorTickEnabled(true);
}

void ABHMouse::HandleOnMeshSleep(UPrimitiveComponent* SleepingComponent, FName Name)
{
	this->SetActorTickEnabled(false);
}





