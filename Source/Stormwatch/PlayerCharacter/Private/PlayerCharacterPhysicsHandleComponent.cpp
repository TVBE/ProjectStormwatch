// Copyright (c) 2022-present Barrelhouse. All rights reserved.
// Written by Nino Saglia & Tim Verberne.

#include "PlayerCharacterPhysicsHandleComponent.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterController.h"

UPlayerCharacterPhysicsHandleComponent::UPlayerCharacterPhysicsHandleComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UPlayerCharacterPhysicsHandleComponent::OnRegister()
{
	checkf(Cast<APlayerCharacter>(GetOwner()), TEXT("%s requires owner to be of type APlayerCharacter."), *GetName());
}

APlayerCharacter* UPlayerCharacterPhysicsHandleComponent::GetPlayerCharacter() const
{
	return static_cast<APlayerCharacter*>(GetOwner());
}

APlayerCharacterController* UPlayerCharacterPhysicsHandleComponent::GetPlayerCharacterController() const
{
	APlayerCharacter* Character {GetPlayerCharacter()};
	AController* Controller {Character->GetController()};

	if (!Controller) { return nullptr; }

	return static_cast<APlayerCharacterController*>(Character->GetController());
}
