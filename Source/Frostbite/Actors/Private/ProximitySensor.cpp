// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite


#include "ProximitySensor.h"

// Sets default values
AProximitySensor::AProximitySensor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AProximitySensor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProximitySensor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

