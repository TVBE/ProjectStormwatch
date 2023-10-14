// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "StormwatchWorldSubystem.h"
#include "BHPlayerCharacter.h"
#include "BHPlayerController.h"

DEFINE_LOG_CATEGORY_CLASS(UStormwatchWorldSubsystem, LogStormwatchWorldSubsystem);

void UStormwatchWorldSubsystem::RegisterPlayerCharacter(ABHPlayerCharacter* Character)
{
	if (Character)
	{
		if (PlayerCharacter)
		{
			UE_LOG(LogStormwatchWorldSubsystem, Warning, TEXT("Tried to register a player character while a player character is already registered to the subsystem."));
			return;
		}
		PlayerCharacter = Character;
	}
	else
	{
		UE_LOG(LogStormwatchWorldSubsystem, Error, TEXT("Could not register player character."));
	}
}

void UStormwatchWorldSubsystem::RegisterPlayerController(ABHPlayerCharacterController* Controller)
{
	if (Controller)
	{
		if (PlayerController)
		{
			UE_LOG(LogStormwatchWorldSubsystem, Warning, TEXT("Tried to register a player controller while a player character is already registered to the subsystem."));
			return;
		}
		PlayerController = Controller;
	}
	else
	{
		UE_LOG(LogStormwatchWorldSubsystem, Error, TEXT("Could not register player controller."));
	}
}

void UStormwatchWorldSubsystem::UnregisterPlayerCharacter(ABHPlayerCharacter* Character)
{
	if (Character)
	{
		if (PlayerCharacter == Character)
		{
			PlayerCharacter = nullptr;
		}
	}
}

void UStormwatchWorldSubsystem::UnregisterPlayerController(ABHPlayerCharacterController* Controller)
{
	if (Controller)
	{
		if (PlayerController == Controller)
		{
			PlayerController = nullptr;
		}
	}
}
