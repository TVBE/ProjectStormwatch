// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHNightstalkerController.h"
#include "BHNightstalker.h"
#include "BHPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_CLASS(ABHNightstalkerController, LogNightstalkerController);

void ABHNightstalkerController::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	BehaviorTickInterval = 1.0f / BehaviorTickRate;
}

void ABHNightstalkerController::BeginPlay()
{
	Super::BeginPlay();

	ACharacter* ControlledCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	PlayerCharacter = Cast<ABHPlayerCharacter>(ControlledCharacter);

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

void ABHNightstalkerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
}

void ABHNightstalkerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (!InPawn) { return; }
	Nightstalker = Cast<ANightstalker>(InPawn);
	UE_LOG(LogNightstalkerController, Log, TEXT("Nightstalker was succesfully possesed by controller: '%s'. "), *Nightstalker->GetName())
}

void ABHNightstalkerController::LogText(const FString& Text)
{
	UE_LOG(LogNightstalkerController, VeryVerbose, TEXT("%s"), *Text);
}

void ABHNightstalkerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!Nightstalker || !PlayerCharacter) { return; }
	
	DistanceToPlayerCharacter = FVector::Dist(Nightstalker->GetActorLocation(), PlayerCharacter->GetActorLocation());
	
	const FVector CurrentLocation = GetPawn()->GetActorLocation();
	
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

void ABHNightstalkerController::BehaviorTick_Implementation(float DeltaSeconds)
{
}

