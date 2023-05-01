// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite


#include "PlayerDragComponent.h"


UPlayerDragComponent::UPlayerDragComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerDragComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPlayerDragComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

