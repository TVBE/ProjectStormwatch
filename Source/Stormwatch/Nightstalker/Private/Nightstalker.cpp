// Copyright (c) 2022-present Barrelhouse. All rights reserved.
// Written by Tim Verberne.

#include "Nightstalker.h"

#include "NightstalkerDirector.h"
#include "NightstalkerMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

DEFINE_LOG_CATEGORY_CLASS(ANightstalker, LogNightstalker);

ANightstalker::ANightstalker()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ANightstalker::PostInitProperties()
{
	Super::PostInitProperties();

	NightstalkerMovementComponent = Cast<UNightstalkerMovementComponent>(GetCharacterMovement());
}

void ANightstalker::BeginPlay()
{
	Super::BeginPlay();

	if (const UWorld* World {GetWorld()})
	{
		if (UNightstalkerDirector* Director {World->GetSubsystem<UNightstalkerDirector>()})
		{
			Director->RegisterNightstalker(this);
		}
	}
}

void ANightstalker::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (const UWorld* World {GetWorld()})
	{
		if (UNightstalkerDirector* Director {World->GetSubsystem<UNightstalkerDirector>()})
		{
			Director->UnregisterNightstalker(this);
		}
	}
	
	Super::EndPlay(EndPlayReason);
}

void ANightstalker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ANightstalker::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ANightstalker::SetPhantomModeEnabled(const bool Value)
{
	if (IsPhantomModeEnabled != Value)
	{
		IsPhantomModeEnabled = Value;
		EventSetPhantomModeEnabled(Value);
	}
}

