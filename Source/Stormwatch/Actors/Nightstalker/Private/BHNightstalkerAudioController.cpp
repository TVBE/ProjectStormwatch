// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHNightstalkerAudioController.h"

// Sets default values for this component's properties
UBHNightstalkerAudioController::UBHNightstalkerAudioController()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UBHNightstalkerAudioController::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UBHNightstalkerAudioController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

