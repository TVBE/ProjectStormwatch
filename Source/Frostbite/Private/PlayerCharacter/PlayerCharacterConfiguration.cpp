// Copyright 2023 Tim Verberne.

#include "PlayerCharacterConfiguration.h"

#include "PlayerCameraController.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterController.h"
#include "PlayerFlashlightController.h"
#include "Camera/CameraComponent.h"
#include "Components/SpotLightComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

void UPlayerCharacterConfiguration::ApplyToPlayerCharacter(const APlayerCharacter* PlayerCharacter)
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

void UPlayerCharacterConfiguration::ApplyToPlayerController(APlayerController* PlayerController)
{
	if(!PlayerController)
	{
		return;
	}

	if(APlayerCharacterController* CharacterController {Cast<APlayerCharacterController>(PlayerController)})
	{
		CharacterController->CharacterConfiguration = this;
	}
}

void UPlayerCameraConfiguration::ApplyToPlayerCharacter(const APlayerCharacter* PlayerCharacter)
{
	if(!PlayerCharacter)
	{
		return;
	}

	if(UPlayerCameraController* CameraController {PlayerCharacter->GetCameraController()})
	{
		CameraController->CameraConfiguration = this;
	}
	
	if(PlayerCharacter->GetCamera())
	{
		PlayerCharacter->GetCamera()->SetFieldOfView(DefaultFOV);
		PlayerCharacter->GetCamera()->PostProcessSettings.VignetteIntensity = DefaultVignetteIntensity;
	}
}

void UPlayerCameraConfiguration::ApplyToPlayerController(const APlayerController* PlayerController)
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

void UPlayerFlashlightConfiguration::ApplyToPlayerCharacter(const APlayerCharacter* PlayerCharacter)
{
	if(!PlayerCharacter)
	{
		return;
	}

	if(UPlayerFlashlightController* FlashlightController {PlayerCharacter->GetFlashlightController()})
	{
		FlashlightController->FlashlightConfiguration = this;
	}
	
	USpotLightComponent* Flashlight {PlayerCharacter->GetFlashlight()};
	USpringArmComponent* FlashlightSpringArm {PlayerCharacter->GetFlashlightSpringArm()};
	
	if(Flashlight && FlashlightSpringArm)
	{
		Flashlight->Intensity = Intensity;
		Flashlight->LightColor = LightColor;
		Flashlight->AttenuationRadius = AttenuationRadius;
		Flashlight->InnerConeAngle = InnerConeAngle;
		Flashlight->OuterConeAngle = OuterConeAngle;
		Flashlight->CastShadows = CastsShadows;
		Flashlight->VolumetricScatteringIntensity = VolumetricScatteringIntensity;
		Flashlight->SetUseInverseSquaredFalloff(UseInverseSquaredFalloff);
		Flashlight->LightFalloffExponent = LightFalloffExponent;
		if(LightFunctionMaterial)
		{
			Flashlight->LightFunctionMaterial = LightFunctionMaterial;
		}
		if(IESTexture)
		{
			Flashlight->IESTexture = IESTexture;
		}

		FlashlightSpringArm->CameraRotationLagSpeed = RotationLag;
	}
}
