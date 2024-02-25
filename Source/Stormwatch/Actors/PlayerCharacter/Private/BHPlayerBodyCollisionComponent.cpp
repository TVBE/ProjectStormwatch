// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHPlayerBodyCollisionComponent.h"

UBHPlayerBodyCollisionComponent::UBHPlayerBodyCollisionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UBHPlayerBodyCollisionComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBHPlayerBodyCollisionComponent::OnRegister()
{
	Super::OnRegister();

	SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	SetCollisionResponseToAllChannels(ECR_Ignore);
	SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Block);
}

void UBHPlayerBodyCollisionComponent::OnUnregister()
{
	Super::OnRegister();

	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetCollisionResponseToAllChannels(ECR_Ignore);
}

void UBHPlayerBodyCollisionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


