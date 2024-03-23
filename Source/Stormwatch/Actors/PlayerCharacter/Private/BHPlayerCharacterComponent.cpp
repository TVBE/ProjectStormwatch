// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "..\Public\BHPlayerCharacterComponent.h"

#include "BHPlayerCharacter.h"
#include "BHPlayerController.h"

ABHPlayerCharacter* FBHPlayerCharacterComponent::GetPlayerCharacter() const
{
	const UActorComponent* ActorComponent = reinterpret_cast<const UActorComponent*>(this);
    
	if (!ActorComponent->GetOwner())
	{
		return nullptr;
	}
		
	return static_cast<ABHPlayerCharacter*>(ActorComponent->GetOwner());
}

ABHPlayerController* FBHPlayerCharacterComponent::GetPlayerCharacterController() const
{
	const ABHPlayerCharacter* Character = GetPlayerCharacter();
	return Cast<ABHPlayerController>(Character ? Character->GetController() : nullptr);
}
