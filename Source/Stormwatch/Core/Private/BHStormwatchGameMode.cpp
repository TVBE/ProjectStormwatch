// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHStormwatchGameMode.h"
#include "BHPlayerCharacter.h"

ABHStormwatchGameMode::ABHStormwatchGameMode()
{

}

void ABHStormwatchGameMode::NotifyPlayerCharacterBeginPlay(ABHPlayerCharacter* Character)
{
	if (Character)
	{
		IsPlayerActive = true;
		EventOnPlayerSpawn(Character);
	}
}

void ABHStormwatchGameMode::EventOnPlayerSpawn_Implementation(ABHPlayerCharacter* Character)
{
}

