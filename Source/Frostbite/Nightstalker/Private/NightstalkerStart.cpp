// Copyright (c) 2022-present Barrelhouse. All rights reserved.
// Written by Tim Verberne.

#include "NightstalkerStart.h"

// Sets default values
ANightstalkerStart::ANightstalkerStart()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ANightstalkerStart::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ANightstalkerStart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

