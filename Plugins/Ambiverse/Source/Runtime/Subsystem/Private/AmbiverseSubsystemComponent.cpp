// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#include "AmbiverseSubsystemComponent.h"

void UAmbiverseSubsystemComponent::Initialize(UAmbiverseSubsystem* Subsystem)
{
	if (!Subsystem) { return; }
	Owner = Subsystem;
	IsComponentInitialized = true;
}

void UAmbiverseSubsystemComponent::Deinitialize(UAmbiverseSubsystem* Subsystem)
{
	if (!Subsystem) { return; }
	Owner = nullptr;
	IsComponentInitialized = false;
}

void UAmbiverseSubsystemComponent::Tick(float DeltaTime)
{
}

