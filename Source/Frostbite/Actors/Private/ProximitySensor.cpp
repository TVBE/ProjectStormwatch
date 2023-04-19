// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#include "ProximitySensor.h"

#include "Nightstalker.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"

DEFINE_LOG_CATEGORY_CLASS(AProximitySensor, LogSensor)

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

#if WITH_EDITOR
	if (EnableDebugVisualisation)
	{
		VisualizeCone(false);
	}
#endif
}

void AProximitySensor::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(PollTimerHandle, this, &AProximitySensor::Poll, PollInterval, true);
#if WITH_EDITOR
	if (EnableDebugVisualisation)
	{
		VisualizeCone(true);
	}
#endif
}

/** Called at regular intervals to check for overlapping actors and determine the nearest pawn. */
void AProximitySensor::Poll()
{
	if (!DetectionBox) { return; }
	DetectionBox->GetOverlappingActors(OverlappingActors, APawn::StaticClass());
	
	if (!OverlappingActors.IsEmpty())
	{
		for (int32 Index {0}; Index < OverlappingActors.Num(); ++Index)
		{
			AActor* Actor {OverlappingActors[Index]};
			bool ShouldIgnoreActor {false};

			if (Actor)
			{
				if (Actor->IsA(APlayerCharacter::StaticClass()))
				{
					if (IgnoreParameters.Contains(EBProximitySensorIgnoreParameter::Player))
					{
						ShouldIgnoreActor = true;
					}
					else if (IgnoreParameters.Contains(EBProximitySensorIgnoreParameter::Crouching))
					{
						if (const APlayerCharacter* PlayerCharacter {Cast<APlayerCharacter>(Actor)};
							const UPlayerCharacterMovementComponent* CharacterMovement {PlayerCharacter->GetPlayerCharacterMovement()})
						{
							if (CharacterMovement->IsCrouching())
							{
								ShouldIgnoreActor = true;
							}
						}
					}
				}
				else if (Actor->IsA(ANightstalker::StaticClass()) && IgnoreParameters.Contains(EBProximitySensorIgnoreParameter::Nightstalker))
				{
					ShouldIgnoreActor = true;
				}

				if (ShouldIgnoreActor)
				{
					OverlappingActors[Index] = nullptr;
				}
				else
				{
					const FVector DirectionToActor {(Actor->GetActorLocation() - this->GetActorLocation()).GetSafeNormal()};
					const float DotProduct {static_cast<float>(FVector::DotProduct(DirectionToActor, Root->GetForwardVector()))};

					if (!(DotProduct > FMath::Cos(FMath::DegreesToRadians(ConeAngle))) || IsActorOccluded(Actor))
					{
						OverlappingActors[Index] = nullptr;
					}
				}
			}
		}
	}

	if (OverlappingActors.IsEmpty()) { return; }

	AActor* NearestActor {nullptr};
	float NearestPawnDistance {FLT_MAX};
	
	for (AActor* OverlappingActor : OverlappingActors)
	{
		if (OverlappingActor)
		{
			const float Distance {static_cast<float>(FVector::Dist(GetActorLocation(), OverlappingActor->GetActorLocation()))};
			if (Distance < NearestPawnDistance)
			{
				NearestPawnDistance = Distance;
				NearestActor = OverlappingActor;
			}
		}
	}

	if (NearestActor)
	{
		IsActorDetected = true;
		OnActorDetected.Broadcast(NearestActor, NearestPawnDistance);
	}
}

/** Determines if the given actor is occluded by another object using a line trace. */
bool AProximitySensor::IsActorOccluded(const AActor* Actor) const
{
	FVector StartLocation {GetActorLocation()};
	FVector ActorLocation {Actor->GetActorLocation()};
	TArray<FVector> EndLocations
	{
		ActorLocation,
		ActorLocation + FVector(0.f, 0.f, 50.f),
		ActorLocation - FVector(0.f, 0.f, 50.f)
	};

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	CollisionParams.AddIgnoredActor(Actor);

	uint8 BlockedTraces {0};

	for (const FVector& EndLocation : EndLocations)
	{
		FHitResult HitResult;

		if (bool IsHit {GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, CollisionParams)})
		{
			AActor* HitActor {HitResult.GetActor()};

			if (HitActor != Actor)
			{
				++BlockedTraces;
			}
		}
	}

	return BlockedTraces == EndLocations.Num();
}

/** Calculates the cone angle based on the dimensions of the given box component. */
float AProximitySensor::CalculateConeAngle(const UBoxComponent* BoxComponent) const 
{
	const FVector BoxExtent {BoxComponent->GetScaledBoxExtent()};
	const float BoxDepth {static_cast<float>(BoxExtent.X)};
	const float Diagonal {static_cast<float>(FMath::Sqrt(FMath::Square(BoxExtent.Y) + FMath::Square(BoxExtent.Z)))};
	const float HalfConeAngleRad = FMath::Atan((Diagonal / 2.0f) / BoxDepth);
	return FMath::RadiansToDegrees(HalfConeAngleRad);
}

#if WITH_EDITOR
/** Visualizes the detection cone by drawing debug lines in the editor. */
void AProximitySensor::VisualizeCone(const bool IsPersistent) const
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

	DrawDebugLine(GetWorld(), BoxLocation, TopRight, DebugLineColor, IsPersistent, 1.0f);
	DrawDebugLine(GetWorld(), BoxLocation, TopLeft, DebugLineColor, IsPersistent, 1.0f);
	DrawDebugLine(GetWorld(), BoxLocation, BottomRight, DebugLineColor, IsPersistent, 1.0f);
	DrawDebugLine(GetWorld(), BoxLocation, BottomLeft, DebugLineColor, IsPersistent, 1.0f);
}
void AProximitySensor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(AProximitySensor, IgnoreParameters))
	{
		TSet<EBProximitySensorIgnoreParameter> UniqueIgnoreParameters;

		for (int Index {0}; Index < IgnoreParameters.Num(); ++Index)
		{
			if (!UniqueIgnoreParameters.Contains(IgnoreParameters[Index]))
			{
				UniqueIgnoreParameters.Add(IgnoreParameters[Index]);
			}
			else
			{
				IgnoreParameters.RemoveAt(Index);
				--Index;
				UE_LOG(LogSensor, Warning, TEXT("Duplicate ignore parameters have been detected and removed."));
			}
		}
	}
}
#endif



