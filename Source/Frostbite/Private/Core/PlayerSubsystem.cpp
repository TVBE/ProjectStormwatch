// Copyright 2023 Barrelhouse


#include "Core/PlayerSubsystem.h"

#include "Core/LogCategories.h"
#include "PlayerCharacter/PlayerCameraController.h"
#include "PlayerCharacter/PlayerCharacter.h"
#include "PlayerCharacter/PlayerCharacterController.h"

void UPlayerSubsystem::RegisterPlayerCharacter(APlayerCharacter* Character)
{
	if(Character)
	{
		if(PlayerCharacter)
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
	if(Controller)
	{
		if(PlayerController)
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

bool UPlayerSubsystem::SetPlayerMovementInputLock(const bool Value)
{
	MovementInputLockCount += Value ? 1 : -1;
	if(PlayerController)
	{
		const bool CanProcessInput {!MovementInputLockCount};
		if(PlayerController->GetCanProcessMovementInput() != CanProcessInput)
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
	if(PlayerController)
	{
		const bool CanProcessInput {!RotationInputLockCount};
		if(PlayerController->GetCanProcessRotationInput() != CanProcessInput)
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
	if(PlayerCharacter && PlayerCharacter->GetCameraController())
	{
		PlayerCharacter->GetCameraController()->FadeFromBlack(Duration);
	}
}
