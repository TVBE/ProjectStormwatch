// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#include "NightstalkerController.h"
#include "Nightstalker.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_CLASS(ANightstalkerController, LogNightstalkerController);

void ANightstalkerController::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	BehaviorTickInterval = 1.0f / BehaviorTickRate;
}

void ANightstalkerController::BeginPlay()
{
	Super::BeginPlay();

	ACharacter* ControlledCharacter {UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)};
	PlayerCharacter = Cast<APlayerCharacter>(ControlledCharacter);

	if (PlayerCharacter)
	{
		UE_LOG(LogNightstalkerController, Verbose, TEXT("Successfully found player character."));
	}
	else
	{
		UE_LOG(LogNightstalkerController, Warning, TEXT("Failed to find player character."));
	}

	GetWorld()->GetTimerManager().SetTimer(BehaviorTickTimerHandle, [this]() { BehaviorTick(BehaviorTickInterval); }, BehaviorTickInterval, true);
}

void ANightstalkerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
}

void ANightstalkerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (!InPawn) { return; }
	Nightstalker = Cast<ANightstalker>(InPawn);
	UE_LOG(LogNightstalkerController, Log, TEXT("Nightstalker was succesfully possesed by controller: '%s'. "), *Nightstalker->GetName())
}

void ANightstalkerController::LogText(const FString& Text)
{
	UE_LOG(LogNightstalkerController, VeryVerbose, TEXT("%s"), *Text);
}

inline static const TArray<FVector2D> OcclusionTraceVectors = {
	FVector2D(-50, 50),
	FVector2D(50, 50),
	FVector2D(-50, -50),
	FVector2D(50, -50)
};

void ANightstalkerController::IsOccluded(bool& ReturnValue, const FVector& PointA, const FVector& PointB, const bool DrawDebugLines, const float DrawDebugLinesDuration)
{
	UWorld* World {GetWorld()};
	if (!World)
	{
		ReturnValue = false;
		return;
	}

	FVector Direction { (PointB - PointA).GetSafeNormal() };
	FRotator Rotation { Direction.Rotation() };

	FCollisionQueryParams TraceParams;

	FHitResult HitResult;
	for (const FVector2D& TraceVector : OcclusionTraceVectors)
	{
		FVector Start { Rotation.RotateVector(FVector{0, TraceVector.X, TraceVector.Y}) + PointA };
		FVector End { Rotation.RotateVector(FVector{0, TraceVector.X, TraceVector.Y}) + PointB };
		
		bool IsHit { World->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, TraceParams) };
		if (!IsHit)
		{
			if (DrawDebugLines)
			{
				DrawDebugLine(World, Start, End, FColor::Red, false, DrawDebugLinesDuration);
			}

			ReturnValue = false;

			if (!DrawDebugLines)
			{
				return;
			}
		}
		else if (DrawDebugLines)
		{
			DrawDebugLine(World, Start, End, FColor::Green, false, DrawDebugLinesDuration);
		}
	}

	ReturnValue = true;
}

void ANightstalkerController::IsOccludedFast(bool& ReturnValue, const FVector& LocationA, const FVector& LocationB, const bool DrawDebugLines, const float DrawDebugLinesDuration)
{
	UWorld* World {GetWorld()};
	if (!World)
	{
		ReturnValue = false;
		return;
	}

	FVector SourceTracePoints[2] {LocationA + FVector(0, 0, 50), LocationA - FVector(0, 0, 30)};
	FVector TargetTracePoints[2] {LocationB + FVector(0, 0, 50), LocationB - FVector(0, 0, 30)};

	FCollisionQueryParams TraceParams {};

	FHitResult HitResult;
	for (int i = 0; i < 2; ++i)
	{
		bool IsHit {World->LineTraceSingleByChannel(HitResult, SourceTracePoints[i], TargetTracePoints[i], ECC_Visibility, TraceParams)};
		if (DrawDebugLines)
		{
			DrawDebugLine(World, SourceTracePoints[i], TargetTracePoints[i], IsHit ? FColor::Red : FColor::Green, false, DrawDebugLinesDuration);
		}
		if (IsHit)
		{
			ReturnValue = false;

			if (!DrawDebugLines)
			{
				return;
			}
		}
	}

	ReturnValue = true;
}

float ANightstalkerController::GetPlayerViewAngleToNightstalker(bool IgnorePitch) const
{
	if (!Nightstalker || !PlayerCharacter)
	{
		return 0.0f; 
	}
	
	FVector DirectionToNightstalker {(Nightstalker->GetActorLocation() - PlayerCharacter->GetActorLocation()).GetSafeNormal()};
	
	FVector PlayerForwardVector {PlayerCharacter->GetControlRotation().Vector()};

	if (IgnorePitch)
	{
		DirectionToNightstalker.Z = 0.0f;
		PlayerForwardVector.Z = 0.0f;
		DirectionToNightstalker = DirectionToNightstalker.GetSafeNormal();
		PlayerForwardVector = PlayerForwardVector.GetSafeNormal();
	}
	
	float Angle {static_cast<float>(FMath::Acos(FVector::DotProduct(DirectionToNightstalker, PlayerForwardVector)) * (180.0f / PI))};

	return Angle;
}

void ANightstalkerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!Nightstalker || !PlayerCharacter) { return; }
	
	DistanceToPlayerCharacter = FVector::Dist(Nightstalker->GetActorLocation(), PlayerCharacter->GetActorLocation());
	
	const FVector CurrentLocation {GetPawn()->GetActorLocation()};
	
	if (FVector::DistSquared(LastRegisteredLocation, CurrentLocation) > 2500.f)
	{
		if (PathHistory.Num() >= MaxPathHistoryLength)
		{
			PathHistory.RemoveAt(0);
		}
		
		PathHistory.Add(CurrentLocation);
		
		LastRegisteredLocation = CurrentLocation;
	}
}

void ANightstalkerController::BehaviorTick_Implementation(float DeltaSeconds)
{
}

