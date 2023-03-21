// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

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
