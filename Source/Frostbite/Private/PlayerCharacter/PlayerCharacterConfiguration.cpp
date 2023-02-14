// Copyright 2023 Tim Verberne.

#include "PlayerCharacterConfiguration.h"

#include "PlayerCharacter.h"
#include "PlayerCharacterController.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void UPlayerCharacterConfiguration::ApplyToPlayerCharacter(APlayerCharacter* PlayerCharacter)
{
	if(!PlayerCharacter)
	{
		return;
	}
	
	// Set character's movement component properties.
	if(UCharacterMovementComponent* MovementComponent {PlayerCharacter->GetCharacterMovement()})
	{
		MovementComponent->MaxWalkSpeed = WalkSpeed;
		MovementComponent->JumpZVelocity = JumpVelocity;
	}
}

void UPlayerCameraConfiguration::ApplyToPlayerCharacter(APlayerCharacter* PlayerCharacter)
{
	if(!PlayerCharacter)
	{
		return;
	}

	if(PlayerCharacter->GetCamera())
	{
		PlayerCharacter->GetCamera()->SetFieldOfView(DefaultFOV);
		PlayerCharacter->GetCamera()->PostProcessSettings.VignetteIntensity = DefaultVignetteIntensity;
	}
}

void UPlayerCameraConfiguration::ApplyToPlayerController(APlayerController* PlayerController)
{
	if(!PlayerController)
	{
		return;
	}
	if(PlayerController->PlayerCameraManager)
	{
		PlayerController->PlayerCameraManager->ViewPitchMax = MaximumViewPitch;
		PlayerController->PlayerCameraManager->ViewPitchMin = MinimumViewPitch;
	}
		
}

void UPlayerFlashlightConfiguration::ApplyToPlayerCharacter(APlayerCharacter* PlayerCharacter)
{
}
