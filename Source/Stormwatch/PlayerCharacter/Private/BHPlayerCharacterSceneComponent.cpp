// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHPlayerCharacterSceneComponent.h"
#include "BHPlayerCharacter.h"
#include "BHPlayerController.h"

UBHPlayerCharacterSceneComponent::UBHPlayerCharacterSceneComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UBHPlayerCharacterSceneComponent::OnRegister()
{
	Super::OnRegister();

	checkf(Cast<ABHPlayerCharacter>(GetOwner()), TEXT("%s requires owner to be of type ABHPlayerCharacter."), *GetName());

	APawn* Pawn {Cast<APawn>(GetOwner())};
	Pawn->ReceiveControllerChangedDelegate.AddDynamic(this, &UBHPlayerCharacterSceneComponent::OnControllerChanged);
}

void UBHPlayerCharacterSceneComponent::OnUnregister()
{
	APawn* Pawn {Cast<APawn>(GetOwner())};
	Pawn->ReceiveControllerChangedDelegate.RemoveDynamic(this, &UBHPlayerCharacterSceneComponent::OnControllerChanged);

	Super::OnUnregister();
}

ABHPlayerCharacter* UBHPlayerCharacterSceneComponent::GetPlayerCharacter() const
{
	return static_cast<ABHPlayerCharacter*>(GetOwner());
}

ABHPlayerCharacterController* UBHPlayerCharacterSceneComponent::GetPlayerCharacterController() const
{
	ABHPlayerCharacter* Character {GetPlayerCharacter()};
	AController* Controller {Character->GetController()};

	if (!Controller) { return nullptr; }

	return static_cast<ABHPlayerCharacterController*>(Controller);
}
