// Copyright (c) 2022-present Barrelhouse. All rights reserved.
// Written by Tim Verberne.

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

