// Copyright 2023 Barrelhouse

#include "Nightstalker.h"

// Sets default values
ANightstalker::ANightstalker()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ANightstalker::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANightstalker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ANightstalker::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

