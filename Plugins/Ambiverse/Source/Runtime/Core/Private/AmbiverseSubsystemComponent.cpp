// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#include "AmbiverseSubsystemComponent.h"

void UAmbiverseSubsystemComponent::Initialize(UAmbiverseSubsystem* Subsystem)
{
	if (!Owner) { return; }
	Owner = Subsystem;
}

void UAmbiverseSubsystemComponent::Deinitialize(UAmbiverseSubsystem* Subsystem)
{
	if (!Owner) { return; }
	Owner = nullptr;
}
