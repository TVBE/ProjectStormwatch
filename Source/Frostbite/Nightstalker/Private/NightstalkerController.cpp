// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#include "NightstalkerController.h"
#include "LogCategories.h"

void ANightstalkerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	switch(BehaviorMode)
	{
	case EBehaviorMode::RoamMode: TickRoamMode();
		break;
	case EBehaviorMode::StalkMode: TickStalkMode();
		break;
	case EBehaviorMode::AmbushMode: TickAmbushMode();
		break;
	}
	
}

void ANightstalkerController::SwitchBehaviorMode(const EBehaviorMode Mode)
{
	if(BehaviorMode == Mode)
	{
		return;
	}
	BehaviorMode = Mode;
	
	FTimerManager& TimerManager {GetWorldTimerManager()};
	/** Clear the update timer. */
	if(BehaviorUpdateTimerHandle.IsValid())
	{
		TimerManager.ClearTimer(BehaviorUpdateTimerHandle);
	}
	
	/** Set the update interval by the behavior mode type. */
	float Interval {1.0f};
	switch(BehaviorMode)
	{
	case EBehaviorMode::RoamMode:
		StartRoamMode();
		Interval = RoamModeUpdateInterval;
		break;
	case EBehaviorMode::StalkMode:
		StartStalkMode();
		Interval = StalkModeUpdateInterval;
		break;
	case EBehaviorMode::AmbushMode:
		StartAmbushMode();
		Interval = AmbushModeUpdateInterval;
		break;
	}
	
	/** Initialize the update timer. */
	TimerManager.SetTimer(BehaviorUpdateTimerHandle, this, &ANightstalkerController::OnBehaviorModeUpdate, Interval, true);
}

void ANightstalkerController::OnBehaviorModeUpdate()
{
	switch(BehaviorMode)
	{
	case EBehaviorMode::RoamMode:
		UpdateRoamMode();
		break;
	case EBehaviorMode::StalkMode:
		UpdateStalkMode();
		break;
	case EBehaviorMode::AmbushMode:
		UpdateAmbushMode();
		break;
	}
}

void ANightstalkerController::TickAmbushMode_Implementation()
{
}

void ANightstalkerController::TickStalkMode_Implementation()
{
}

void ANightstalkerController::TickRoamMode_Implementation()
{
}

void ANightstalkerController::UpdateAmbushMode_Implementation()
{
}

void ANightstalkerController::UpdateStalkMode_Implementation()
{
}

void ANightstalkerController::UpdateRoamMode_Implementation()
{
}

void ANightstalkerController::StartAmbushMode_Implementation()
{
}

void ANightstalkerController::StartStalkMode_Implementation()
{
}

void ANightstalkerController::StartRoamMode_Implementation()
{
}

void ANightstalkerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}