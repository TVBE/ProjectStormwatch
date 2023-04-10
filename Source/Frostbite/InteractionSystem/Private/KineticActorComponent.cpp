// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#include "KineticActorComponent.h"

UKineticActorComponent::UKineticActorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;


}

void UKineticActorComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UKineticActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


}

