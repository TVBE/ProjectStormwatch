// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#include "ProximitySensor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

AProximitySensor::AProximitySensor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	DetectionCone = CreateDefaultSubobject<UCapsuleComponent>(TEXT("DetectionCone"));
	SetRootComponent(DetectionCone);
	DetectionCone->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DetectionCone->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	DetectionCone->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	DetectionCone->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void AProximitySensor::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(PollTimerHandle, this, &AProximitySensor::Poll, PollInterval, true);
}

void AProximitySensor::Poll()
{
	TArray<AActor*> OverlappingActors;
	DetectionCone->GetOverlappingActors(OverlappingActors, APawn::StaticClass());

	APawn* NearestPawn = nullptr;
	float NearestPawnDistance = FLT_MAX;

	for (AActor* OverlappingActor : OverlappingActors)
	{
		if (APawn* OverlappingPawn = Cast<APawn>(OverlappingActor))
		{
			float Distance = FVector::Distance(GetActorLocation(), OverlappingPawn->GetActorLocation());
			if (Distance < NearestPawnDistance)
			{
				NearestPawnDistance = Distance;
				NearestPawn = OverlappingPawn;
			}
		}
	}

	if (NearestPawn)
	{
		IsPawnDetected = true;
		OnPawnDetectedDelegate.Broadcast(NearestPawn, NearestPawnDistance);
	}
}


