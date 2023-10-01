// Copyright (c) 2022-present Barrelhouse. All rights reserved.
// Written by Tim Verberne.

#include "PlayerCharacterComponent.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterController.h"

UPlayerCharacterComponent::UPlayerCharacterComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UPlayerCharacterComponent::OnRegister()
{
	Super::OnRegister();

	checkf(Cast<APlayerCharacter>(GetOwner()), TEXT("%s requires owner to be of type APlayerCharacter."), *GetName());

	APawn* Pawn {Cast<APawn>(GetOwner())};
	Pawn->ReceiveControllerChangedDelegate.AddDynamic(this, &UPlayerCharacterComponent::OnControllerChanged);
}

void UPlayerCharacterComponent::OnUnregister()
{
	APawn* Pawn {Cast<APawn>(GetOwner())};
	Pawn->ReceiveControllerChangedDelegate.RemoveDynamic(this, &UPlayerCharacterComponent::OnControllerChanged);

	Super::OnUnregister();
}

APlayerCharacter* UPlayerCharacterComponent::GetPlayerCharacter() const
{
	return static_cast<APlayerCharacter*>(GetOwner());
}

APlayerCharacterController* UPlayerCharacterComponent::GetPlayerCharacterController() const
{
	APlayerCharacter* Character {GetPlayerCharacter()};
	AController* Controller {Character->GetController()};

	if (!Controller) { return nullptr; }

	return static_cast<APlayerCharacterController*>(Controller);
}
