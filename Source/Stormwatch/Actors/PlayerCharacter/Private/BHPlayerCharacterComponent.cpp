// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHPlayerCharacterComponent.h"

#include "BHPlayerCharacter.h"
#include "BHPlayerController.h"

UBHPlayerCharacterComponent::UBHPlayerCharacterComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UBHPlayerCharacterComponent::OnRegister()
{
	Super::OnRegister();

	checkf(Cast<ABHPlayerCharacter>(GetOwner()), TEXT("%s requires owner to be of type ABHPlayerCharacter."), *GetName());

	APawn* Pawn = Cast<APawn>(GetOwner());
	Pawn->ReceiveControllerChangedDelegate.AddDynamic(this, &UBHPlayerCharacterComponent::OnControllerChanged);
}

void UBHPlayerCharacterComponent::OnUnregister()
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	Pawn->ReceiveControllerChangedDelegate.RemoveDynamic(this, &UBHPlayerCharacterComponent::OnControllerChanged);

	Super::OnUnregister();
}

ABHPlayerCharacter* UBHPlayerCharacterComponent::GetPlayerCharacter() const
{
	return static_cast<ABHPlayerCharacter*>(GetOwner());
}

ABHPlayerController* UBHPlayerCharacterComponent::GetPlayerCharacterController() const
{
	ABHPlayerCharacter* Character = GetPlayerCharacter();
	AController* Controller = Character->GetController();

	if (!Controller) { return nullptr; }

	return static_cast<ABHPlayerController*>(Controller);
}
