// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#include "FrostbiteWorldSubystem.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterController.h"

DEFINE_LOG_CATEGORY_CLASS(UFrostbiteWorldSubsystem, LogFrostbiteWorldSubsystem);

void UFrostbiteWorldSubsystem::RegisterPlayerCharacter(APlayerCharacter* Character)
{
	if (Character)
	{
		if (PlayerCharacter)
		{
			UE_LOG(LogFrostbiteWorldSubsystem, Warning, TEXT("Tried to register a player character while a player character is already registered to the subsystem."));
			return;
		}
		PlayerCharacter = Character;
	}
	else
	{
		UE_LOG(LogFrostbiteWorldSubsystem, Error, TEXT("Could not register player character."));
	}
}

void UFrostbiteWorldSubsystem::RegisterPlayerController(APlayerCharacterController* Controller)
{
	if (Controller)
	{
		if (PlayerController)
		{
			UE_LOG(LogFrostbiteWorldSubsystem, Warning, TEXT("Tried to register a player controller while a player character is already registered to the subsystem."));
			return;
		}
		PlayerController = Controller;
	}
	else
	{
		UE_LOG(LogFrostbiteWorldSubsystem, Error, TEXT("Could not register player controller."));
	}
}

void UFrostbiteWorldSubsystem::UnregisterPlayerCharacter(APlayerCharacter* Character)
{
	if (Character)
	{
		if (PlayerCharacter == Character)
		{
			PlayerCharacter = nullptr;
		}
	}
}

void UFrostbiteWorldSubsystem::UnregisterPlayerController(APlayerCharacterController* Controller)
{
	if (Controller)
	{
		if (PlayerController == Controller)
		{
			PlayerController = nullptr;
		}
	}
}
