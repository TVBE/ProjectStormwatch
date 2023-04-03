// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#include "ProximitySensor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"

AProximitySensor::AProximitySensor()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	DetectionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("DetectionBox"));
	DetectionBox->SetupAttachment(this->RootComponent);
	DetectionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DetectionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	DetectionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	DetectionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void AProximitySensor::PostInitProperties()
{
	Super::PostInitProperties();
}

void AProximitySensor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	DetectionBox->SetBoxExtent(FVector(DetectionBoxLength, DetectionBoxWidth, DetectionBoxHeighth), false);
	DetectionBox->SetRelativeLocation(FVector(DetectionBoxLength, 0, 0));
	ConeAngle = CalculateConeAngle(DetectionBox);
}

void AProximitySensor::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(PollTimerHandle, this, &AProximitySensor::Poll, PollInterval, true);
#if WITH_EDITOR
	if (EnableDebugVisualisation)
	{
		VisualizeCone(MAX_FLT);
	}
#endif
}

void AProximitySensor::Poll()
{
	DetectionBox->GetOverlappingActors(OverlappingActors, APawn::StaticClass());
	
	if (!OverlappingActors.IsEmpty())
	{
		for (int32 Index {0}; Index < OverlappingActors.Num(); ++Index)
		{
			if (const AActor* Actor = OverlappingActors[Index])
			{
				const FVector DirectionToActor {(Actor->GetActorLocation() - this->GetActorLocation()).GetSafeNormal()};
				const float DotProduct {static_cast<float>(FVector::DotProduct(DirectionToActor, Root->GetForwardVector()))};

				if (!(DotProduct > FMath::Cos(FMath::DegreesToRadians(ConeAngle))))
				{
					OverlappingActors[Index] = nullptr;
				}
			}
		}
	}

	if (OverlappingActors.IsEmpty()) { return; }

	APawn* NearestPawn {nullptr};
	float NearestPawnDistance {FLT_MAX};
	
	for (AActor* OverlappingActor : OverlappingActors)
	{
		if (APawn* OverlappingPawn = Cast<APawn>(OverlappingActor))
		{
			const float Distance {static_cast<float>(FVector::Dist(GetActorLocation(), OverlappingPawn->GetActorLocation()))};
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

float AProximitySensor::CalculateConeAngle(const UBoxComponent* BoxComponent) const 
{
	const FVector BoxExtent {BoxComponent->GetScaledBoxExtent()};
	const float BoxDepth {static_cast<float>(BoxExtent.X)};
	const float Diagonal {static_cast<float>(FMath::Sqrt(FMath::Square(BoxExtent.Y) + FMath::Square(BoxExtent.Z)))};
	const float HalfConeAngleRad = FMath::Atan((Diagonal / 2.0f) / BoxDepth);
	return FMath::RadiansToDegrees(HalfConeAngleRad);
}

#if WITH_EDITOR
void AProximitySensor::VisualizeCone(const float DebugLineDuration) const
{
	const FVector BoxExtent {DetectionBox->GetScaledBoxExtent()};
	const FVector BoxLocation {Root->GetComponentLocation()};
	const FVector BoxForward {DetectionBox->GetForwardVector()};

	const float ConeDistance {static_cast<float>(BoxExtent.X * 2.0f)};
	const float ConeRadius {FMath::Tan(FMath::DegreesToRadians(ConeAngle) * 0.5f) * ConeDistance};

	const FVector ConeCenter {BoxLocation + BoxForward * ConeDistance};
	const FVector ConeUp {DetectionBox->GetUpVector()};
	const FVector ConeRight {DetectionBox->GetRightVector()};

	const FVector TopRight {ConeCenter + ConeUp * ConeRadius + ConeRight * ConeRadius};
	const FVector TopLeft {ConeCenter + ConeUp * ConeRadius - ConeRight * ConeRadius};
	const FVector BottomRight {ConeCenter - ConeUp * ConeRadius + ConeRight * ConeRadius};
	const FVector BottomLeft {ConeCenter - ConeUp * ConeRadius - ConeRight * ConeRadius};

	const FColor DebugLineColor {FColor::Red};

	DrawDebugLine(GetWorld(), BoxLocation, TopRight, DebugLineColor, false, DebugLineDuration);
	DrawDebugLine(GetWorld(), BoxLocation, TopLeft, DebugLineColor, false, DebugLineDuration);
	DrawDebugLine(GetWorld(), BoxLocation, BottomRight, DebugLineColor, false, DebugLineDuration);
	DrawDebugLine(GetWorld(), BoxLocation, BottomLeft, DebugLineColor, false, DebugLineDuration);
}
#endif



