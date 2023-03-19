﻿// Copyright 2023 Tim Verberne.

#include "PlayerCharacterConfiguration.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterController.h"
#include "PlayerFlashlightComponent.h"

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

void UPlayerCameraConfiguration::ApplyToCamera(UCameraComponent* Camera)
{
	if(!Camera) {return; }
	Camera->SetFieldOfView(DefaultFOV);
	Camera->PostProcessSettings.VignetteIntensity = DefaultVignetteIntensity;
}

void UPlayerFlashlightConfiguration::ApplyToFlashlightComponent(const UPlayerFlashlightComponent* Component)
{
	USpotLightComponent* Flashlight {Component->GetFlashlight()};
	USpringArmComponent* FlashlightSpringArm {Component->GetFlashlightSpringArm()};
	
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

