// Copyright 2023 Barrelhouse


#include "FrostbiteGameMode.h"

#include "PlayerCharacter/PlayerCharacter.h"

void AFrostbiteGameMode::NotifyPlayerCharacterBeginPlay(APlayerCharacter* Character)
{
	if(Character)
	{
		IsPlayerActive = true;
		OnPlayerSpawn(Character);
	}
}

void AFrostbiteGameMode::OnPlayerSpawn_Implementation(APlayerCharacter* Character)
{
	
}


