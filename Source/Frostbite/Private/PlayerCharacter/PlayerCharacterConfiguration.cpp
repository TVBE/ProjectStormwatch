// Copyright 2023 Tim Verberne.

#include "PlayerCharacterConfiguration.h"

#include "PlayerCharacter.h"
#include "PlayerCharacterController.h"

void FPlayerCharacterConfiguration::ValidateData()
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

void FPlayerCharacterConfiguration::ApplyToPlayerCharacterInstance(APlayerCharacter* PlayerCharacter, APlayerCharacterController* PlayerControler)
{
	if(!PlayerCharacter)
	{
		return;
	}
	// Restrict the CameraManager view pitch.
	if(PlayerControler && PlayerControler->PlayerCameraManager)
	{
		PlayerControler->PlayerCameraManager->ViewPitchMax = MaximumViewPitch;
		PlayerControler->PlayerCameraManager->ViewPitchMin = MinimumViewPitch;
	}
	// Set character's movement component properties.
	if(UCharacterMovementComponent* MovementComponent {PlayerCharacter->GetCharacterMovement()})
	{
		MovementComponent->MaxWalkSpeed = WalkSpeed;
		MovementComponent->JumpZVelocity = JumpVelocity;
	}
	
	
}
