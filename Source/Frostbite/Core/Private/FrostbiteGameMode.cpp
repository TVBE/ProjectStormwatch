// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#include "FrostbiteGameMode.h"
#include "PlayerCharacter.h"

void AFrostbiteGameMode::NotifyPlayerCharacterBeginPlay(APlayerCharacter* Character)
{
	if (Character)
	{
		IsPlayerActive = true;
		EventOnPlayerSpawn(Character);
	}
}

void AFrostbiteGameMode::EventOnPlayerSpawn_Implementation(APlayerCharacter* Character)
{
}


