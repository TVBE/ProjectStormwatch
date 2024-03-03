// Copyright (c) 2022-present Barrelhouse. All rights reserved.
// Written by Tim Verberne.

#include "Blizzard.h"

// Sets default values
ABlizzard::ABlizzard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABlizzard::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABlizzard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

