// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "PlayerCharacterSceneComponent.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterController.h"

UPlayerCharacterSceneComponent::UPlayerCharacterSceneComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UPlayerCharacterSceneComponent::OnRegister()
{
	Super::OnRegister();

	checkf(Cast<APlayerCharacter>(GetOwner()), TEXT("%s requires owner to be of type APlayerCharacter."), *GetName());

	APawn* Pawn {Cast<APawn>(GetOwner())};
	Pawn->ReceiveControllerChangedDelegate.AddDynamic(this, &UPlayerCharacterSceneComponent::OnControllerChanged);
}

void UPlayerCharacterSceneComponent::OnUnregister()
{
	APawn* Pawn {Cast<APawn>(GetOwner())};
	Pawn->ReceiveControllerChangedDelegate.RemoveDynamic(this, &UPlayerCharacterSceneComponent::OnControllerChanged);

	Super::OnUnregister();
}

APlayerCharacter* UPlayerCharacterSceneComponent::GetPlayerCharacter() const
{
	return static_cast<APlayerCharacter*>(GetOwner());
}

APlayerCharacterController* UPlayerCharacterSceneComponent::GetPlayerCharacterController() const
{
	APlayerCharacter* Character {GetPlayerCharacter()};
	AController* Controller {Character->GetController()};

	if (!Controller) { return nullptr; }

	return static_cast<APlayerCharacterController*>(Controller);
}
