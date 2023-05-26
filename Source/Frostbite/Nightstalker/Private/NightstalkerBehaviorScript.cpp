// Copyright (c) 2022-present Barrelhouse. All rights reserved.
// Written by Tim Verberne.

#include "NightstalkerBehaviorScript.h"

UNightstalkerBehaviorScript::UNightstalkerBehaviorScript()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UNightstalkerBehaviorScript::ActivateScript()
{
	if (IsScriptActive) {return; }

	SetComponentTickEnabled(true);
	
	IsScriptActive = true;
	OnActivation.Broadcast();
}

void UNightstalkerBehaviorScript::DeactivateScript()
{
	if (!IsScriptActive) {return; }

	SetComponentTickEnabled(false);
	
	IsScriptActive = false;
	OnDeactivation.Broadcast();
}

void UNightstalkerBehaviorScript::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
