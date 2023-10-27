// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHNightstalkerAudioComponent.h"

UBHNightstalkerAudioComponent::UBHNightstalkerAudioComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UBHNightstalkerAudioComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UBHNightstalkerAudioComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

