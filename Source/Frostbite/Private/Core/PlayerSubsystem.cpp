// Copyright 2023 Barrelhouse


#include "Core/PlayerSubsystem.h"

#include "Frostbite/Frostbite.h"
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
		if(!Character->GetController())
		{
			
			UE_LOG(LogPlayerSubsystem, Error, TEXT("Could not get reference to the player character controller."));
			return;
		}
		if(APlayerCharacterController* CharacterController {Cast<APlayerCharacterController>(Character->GetController())})
		{
			PlayerController = CharacterController;
			UE_LOG(LogPlayerSubsystem, Log, TEXT("Successfully registered player character"));
		}
	}
	else
	{
		UE_LOG(LogPlayerSubsystem, Error, TEXT("Could not register player character."));
	}
}

void UPlayerSubsystem::SetPlayerInputEnabled(const bool Value)
{
	if(PlayerController)
	{
		PlayerController->CanProcessMovementInput = Value;
	}
}

void UPlayerSubsystem::FadePlayerCameraFromBlack(const float Duration)
{
	if(PlayerCharacter && PlayerCharacter->GetCameraController())
	{
		PlayerCharacter->GetCameraController()->FadeFromBlack(Duration);
	}
}
