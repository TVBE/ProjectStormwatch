// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHPlayerBodyComponent.h"

UBHPlayerBodyComponent::UBHPlayerBodyComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UBHPlayerBodyComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBHPlayerBodyComponent::OnRegister()
{
	Super::OnRegister();

	SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	SetCollisionResponseToAllChannels(ECR_Ignore);
	SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Block);
}

void UBHPlayerBodyComponent::OnUnregister()
{
	Super::OnRegister();

	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetCollisionResponseToAllChannels(ECR_Ignore);
}

void UBHPlayerBodyComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


