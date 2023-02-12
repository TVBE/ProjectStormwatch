// Copyright 2023 Tim Verberne.

#include "PlayerCharacterConfiguration.h"

#include "PlayerCharacter.h"
#include "PlayerCharacterController.h"

void FPlayerCharacterConfigurationData::ValidateData()
{
	if(IsSprintingEnabled && WalkSpeed >= SprintSpeed)
	{
		IsSprintingEnabled = false;
		UE_LOG(LogTemp, Warning, TEXT("Character SprintSpeed is set lower than WalkSpeed. Sprinting is disabled."))
	}
	/** if(MaximumViewPitch < MinimumViewPitch)
	{
		
	} */
}

void FPlayerCharacterConfigurationData::ApplyToPlayerCharacterInstance(APlayerCharacter* PlayerCharacter)
{
	if(!PlayerCharacter)
	{
		return;
	}
	if(PlayerCharacter->GetController())
	{
		if(const APlayerController* PlayerController {Cast<APlayerController>(PlayerCharacter->GetController())})
		{
			if(PlayerController->PlayerCameraManager)
			{
				PlayerController->PlayerCameraManager->ViewPitchMax = MaximumViewPitch;
				PlayerController->PlayerCameraManager->ViewPitchMin = MinimumViewPitch;
			}
		}
	}
	
	// Set character's movement component properties.
	if(UCharacterMovementComponent* MovementComponent {PlayerCharacter->GetCharacterMovement()})
	{
		MovementComponent->MaxWalkSpeed = WalkSpeed;
		MovementComponent->JumpZVelocity = JumpVelocity;
	}
}
