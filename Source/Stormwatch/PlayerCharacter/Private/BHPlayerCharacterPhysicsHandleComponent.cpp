// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHPlayerCharacterPhysicsHandleComponent.h"
#include "BHPlayerCharacter.h"
#include "BHPlayerController.h"

UBHPlayerCharacterPhysicsHandleComponent::UBHPlayerCharacterPhysicsHandleComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UBHPlayerCharacterPhysicsHandleComponent::OnRegister()
{
	Super::OnRegister();

	checkf(Cast<ABHPlayerCharacter>(GetOwner()), TEXT("%s requires owner to be of type ABHPlayerCharacter."), *GetName());

	APawn* Pawn = Cast<APawn>(GetOwner());
	Pawn->ReceiveControllerChangedDelegate.AddDynamic(this, &UBHPlayerCharacterPhysicsHandleComponent::OnControllerChanged);
}

void UBHPlayerCharacterPhysicsHandleComponent::OnUnregister()
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	Pawn->ReceiveControllerChangedDelegate.RemoveDynamic(this, &UBHPlayerCharacterPhysicsHandleComponent::OnControllerChanged);

	Super::OnUnregister();
}

ABHPlayerCharacter* UBHPlayerCharacterPhysicsHandleComponent::GetPlayerCharacter() const
{
	return static_cast<ABHPlayerCharacter*>(GetOwner());
}

ABHPlayerCharacterController* UBHPlayerCharacterPhysicsHandleComponent::GetPlayerCharacterController() const
{
	ABHPlayerCharacter* Character = GetPlayerCharacter();
	AController* Controller = Character->GetController();

	if (!Controller) { return nullptr; }

	return static_cast<ABHPlayerCharacterController*>(Controller);
}
