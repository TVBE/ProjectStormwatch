// Copyright (c) 2022-present Barrelhouse. All rights reserved.

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
	Super::OnRegister();

	checkf(Cast<APlayerCharacter>(GetOwner()), TEXT("%s requires owner to be of type APlayerCharacter."), *GetName());

	APawn* Pawn {Cast<APawn>(GetOwner())};
	Pawn->ReceiveControllerChangedDelegate.AddDynamic(this, &UPlayerCharacterPhysicsHandleComponent::OnControllerChanged);
}

void UPlayerCharacterPhysicsHandleComponent::OnUnregister()
{
	APawn* Pawn {Cast<APawn>(GetOwner())};
	Pawn->ReceiveControllerChangedDelegate.RemoveDynamic(this, &UPlayerCharacterPhysicsHandleComponent::OnControllerChanged);

	Super::OnUnregister();
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

	return static_cast<APlayerCharacterController*>(Controller);
}
