// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHNightstalker.h"

#include "BHNightstalkerDirector.h"
#include "BHNightstalkerMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

DEFINE_LOG_CATEGORY_CLASS(ABHNightstalker, LogNightstalker);

ABHNightstalker::ABHNightstalker()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABHNightstalker::PostInitProperties()
{
	Super::PostInitProperties();

	NightstalkerMovementComponent = Cast<UBHNightstalkerMovementComponent>(GetCharacterMovement());
}

void ABHNightstalker::BeginPlay()
{
	Super::BeginPlay();

	if (const UWorld* World = GetWorld())
	{
		if (UBHNightstalkerDirector* Director = World->GetSubsystem<UBHNightstalkerDirector>())
		{
			Director->RegisterNightstalker(this);
		}
	}
}

void ABHNightstalker::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (const UWorld* World = GetWorld())
	{
		if (UBHNightstalkerDirector* Director = World->GetSubsystem<UBHNightstalkerDirector>())
		{
			Director->UnregisterNightstalker(this);
		}
	}
	
	Super::EndPlay(EndPlayReason);
}

void ABHNightstalker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABHNightstalker::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABHNightstalker::SetPhantomModeEnabled(const bool Value)
{
	if (IsPhantomModeEnabled != Value)
	{
		IsPhantomModeEnabled = Value;
		EventSetPhantomModeEnabled(Value);
	}
}

