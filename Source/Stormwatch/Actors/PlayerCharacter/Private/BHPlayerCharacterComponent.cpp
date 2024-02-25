// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "..\Public\BHPlayerCharacterComponent.h"

#include "BHPlayerCharacter.h"
#include "BHPlayerController.h"

ABHPlayerCharacter* UBHPlayerCharacterComponent_Deprecated::GetPlayerCharacter() const
{
	return static_cast<ABHPlayerCharacter*>(GetOwner());
}

ABHPlayerController* UBHPlayerCharacterComponent_Deprecated::GetPlayerCharacterController() const
{
	ABHPlayerCharacter* Character = GetPlayerCharacter();
	AController* Controller = Character->GetController();

	if (!Controller) { return nullptr; }

	return static_cast<ABHPlayerController*>(Controller);
}

ABHPlayerCharacter* FBHPlayerCharacterComponent::GetPlayerCharacter() const
{
	const UActorComponent* This = reinterpret_cast<const UActorComponent*>(this);
    
	if (!This->GetOwner())
	{
		return nullptr;
	}
		
	return static_cast<ABHPlayerCharacter*>(This->GetOwner());
}

ABHPlayerController* FBHPlayerCharacterComponent::GetPlayerCharacterController() const
{
	ABHPlayerCharacter* Character = GetPlayerCharacter();
	AController* Controller = Character ? Character->GetController() : nullptr;

	if (!Controller) { return nullptr; }

	return static_cast<ABHPlayerController*>(Controller);
}
