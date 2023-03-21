// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#include "PlayerSubsystem.h"
#include "LogCategories.h"
#include "PlayerCameraController.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterController.h"

void UPlayerSubsystem::RegisterPlayerCharacter(APlayerCharacter* Character)
{
	if (Character)
	{
		if (PlayerCharacter)
		{
			UE_LOG(LogPlayerSubsystem, Warning, TEXT("Tried to register a player character while a player character is already registered to the subsystem."));
			return;
		}
		PlayerCharacter = Character;
	}
	else
	{
		UE_LOG(LogPlayerSubsystem, Error, TEXT("Could not register player character."));
	}
}

void UPlayerSubsystem::RegisterPlayerController(APlayerCharacterController* Controller)
{
	if (Controller)
	{
		if (PlayerController)
		{
			UE_LOG(LogPlayerSubsystem, Warning, TEXT("Tried to register a player controller while a player character is already registered to the subsystem."));
			return;
		}
		PlayerController = Controller;
	}
	else
	{
		UE_LOG(LogPlayerSubsystem, Error, TEXT("Could not register player controller."));
	}
}

void UPlayerSubsystem::UnregisterPlayerCharacter(APlayerCharacter* Character)
{
	if (Character)
	{
		if (PlayerCharacter == Character)
		{
			PlayerCharacter = nullptr;
		}
	}
}

void UPlayerSubsystem::UnregisterPlayerController(APlayerCharacterController* Controller)
{
	if (Controller)
	{
		if (PlayerController == Controller)
		{
			PlayerController = nullptr;
		}
	}
}

bool UPlayerSubsystem::SetPlayerMovementInputLock(const bool Value)
{
	MovementInputLockCount += Value ? 1 : -1;
	if (PlayerController)
	{
		const bool CanProcessInput {!MovementInputLockCount};
		if (PlayerController->GetCanProcessMovementInput() != CanProcessInput)
		{
			PlayerController->SetCanProcessMovementInput(this, CanProcessInput);
			OnMovementInputLockChanged.Broadcast(CanProcessInput);
		}
		return CanProcessInput;
	}
	return false;
}

bool UPlayerSubsystem::SetPlayerRotationInputLock(const bool Value)
{
	RotationInputLockCount += Value ? 1 : -1;
	if (PlayerController)
	{
		const bool CanProcessInput {!RotationInputLockCount};
		if (PlayerController->GetCanProcessRotationInput() != CanProcessInput)
		{
			PlayerController->SetCanProcessRotationInput(this, CanProcessInput);
			OnRotationInputLockChanged.Broadcast(CanProcessInput);
		}
		return CanProcessInput;
	}
	return false;
}

void UPlayerSubsystem::FadePlayerCameraFromBlack(const float Duration)
{
	if (PlayerCharacter && PlayerCharacter->GetCameraController())
	{
		PlayerCharacter->GetCameraController()->FadeFromBlack(Duration);
	}
}
