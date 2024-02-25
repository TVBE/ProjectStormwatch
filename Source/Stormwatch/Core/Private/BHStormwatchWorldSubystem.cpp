// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "..\Public\BHStormwatchWorldSubystem.h"
#include "BHPlayerCharacter.h"
#include "BHPlayerController.h"

DEFINE_LOG_CATEGORY_STATIC(LogBHStormwatchWorldSubsystem, Log, All)

UBHStormwatchWorldSubsystem* UBHStormwatchWorldSubsystem::Get(const UObject* WorldContextObject)
{
	if (!IsValid(WorldContextObject))
	{
		UE_LOG(LogBHStormwatchWorldSubsystem, Warning, TEXT("Tried to get the Stormwatch world subsystem "
			TEXT("from an invalid world context object.")))
		return nullptr;
	}
	return WorldContextObject->GetWorld() ? WorldContextObject->GetWorld()->GetSubsystem<UBHStormwatchWorldSubsystem>()
		: nullptr;
}

void UBHStormwatchWorldSubsystem::Register(ABHPlayerCharacter* Character)
{
	if (!IsValid(Character))
	{
		UE_LOG(LogBHStormwatchWorldSubsystem, Warning, TEXT("Register: No valid character provided."))
	}
	if (UBHStormwatchWorldSubsystem* Subsystem = UBHStormwatchWorldSubsystem::Get(Character))
	{
		if (IsValid(Subsystem->GetPlayerCharacter()))
		{
			UE_LOG(LogBHStormwatchWorldSubsystem, Warning, TEXT("Register: Tried to register a player character ")
				TEXT("while a player character is already registered to the subsystem."));
			return;
		}
		Subsystem->SetPlayerCharacter(Character);
		UE_LOG(LogBHStormwatchWorldSubsystem, Log, TEXT("Successfully registered player character."))
	}
}

void UBHStormwatchWorldSubsystem::Register(ABHPlayerController* Controller)
{
	if (!IsValid(Controller))
	{
		UE_LOG(LogBHStormwatchWorldSubsystem, Warning, TEXT("Register: No valid controller provided."))
	}
	if (UBHStormwatchWorldSubsystem* Subsystem = UBHStormwatchWorldSubsystem::Get(Controller))
	{
		if (IsValid(Subsystem->GetPlayerController()))
		{
			UE_LOG(LogBHStormwatchWorldSubsystem, Warning, TEXT("Register: Tried to register a player character ")
				TEXT("while a player controller is already registered to the subsystem."));
			return;
		}
		Subsystem->SetPlayerController(Controller);
	}
}

void UBHStormwatchWorldSubsystem::Unregister(const ABHPlayerCharacter* Character)
{
	if (!IsValid(Character))
	{
		UE_LOG(LogBHStormwatchWorldSubsystem, Warning, TEXT("Unregister: No valid character provided."))
	}
	if (UBHStormwatchWorldSubsystem* Subsystem = UBHStormwatchWorldSubsystem::Get(Character))
	{
		if (IsValid(Subsystem->GetPlayerCharacter()))
		{
			Subsystem->SetPlayerCharacter(nullptr);
			UE_LOG(LogBHStormwatchWorldSubsystem, Log, TEXT("Successfully unregistered player character."))
		}
		else
		{
			UE_LOG(LogBHStormwatchWorldSubsystem, Warning, TEXT("Unregister: Tried to unregister a player character "
				TEXT("while no character is currently registered. ")))
		}
	}
}

void UBHStormwatchWorldSubsystem::Unregister(const ABHPlayerController* Controller)
{
	if (!IsValid(Controller))
	{
		UE_LOG(LogBHStormwatchWorldSubsystem, Warning, TEXT("Unregister: No valid controller provided."))
	}
	if (UBHStormwatchWorldSubsystem* Subsystem = UBHStormwatchWorldSubsystem::Get(Controller))
	{
		if (IsValid(Subsystem->GetPlayerCharacter()))
		{
			Subsystem->SetPlayerController(nullptr);
			UE_LOG(LogBHStormwatchWorldSubsystem, Log, TEXT("Successfully unregistered player controller."))
		}
		else
		{
			UE_LOG(LogBHStormwatchWorldSubsystem, Warning, TEXT("Unregister: Tried to unregister a player controller "
				TEXT("while no controller is currently registered. ")))
		}
	}
}

void UBHStormwatchWorldSubsystem::SetPlayerCharacter(ABHPlayerCharacter* Character)
{
	PlayerCharacter = Character;
}

void UBHStormwatchWorldSubsystem::SetPlayerController(ABHPlayerController* Controller)
{
	PlayerController = Controller;
}
