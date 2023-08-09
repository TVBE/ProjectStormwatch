// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#include "SubsystemComponent.h"

void USubsystemComponent::Initialize(UAmbiverseSubsystem* Subsystem)
{
	if (!Subsystem) { return; }
	Owner = Subsystem;
	IsComponentInitialized = true;
}

void USubsystemComponent::Deinitialize(UAmbiverseSubsystem* Subsystem)
{
	if (!Subsystem) { return; }
	Owner = nullptr;
	IsComponentInitialized = false;
}

void USubsystemComponent::Tick(float DeltaTime)
{
}

