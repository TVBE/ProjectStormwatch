// Copyright 2023 Barrelhouse


#include "Nighstalker.h"

// Sets default values
ANighstalker::ANighstalker()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ANighstalker::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANighstalker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ANighstalker::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

