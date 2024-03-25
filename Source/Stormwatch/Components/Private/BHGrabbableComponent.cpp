// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHGrabbableComponent.h"

UBHGrabbableComponent::UBHGrabbableComponent()
{
#if WITH_EDITOR
	bAllowOnlyOneInstancePerActor = true;
#endif
}

bool UBHGrabbableComponent::Grab_Implementation(const AActor* Instigator)
{
	BeginInteraction();
	return true;
}

void UBHGrabbableComponent::Release_Implementation(const AActor* Instigator)
{
	EndInteraction();
}

void UBHGrabbableComponent::Equip_Implementation(const AActor* Instigator)
{
}

void UBHGrabbableComponent::Unequip_Implementation(const AActor* Instigator)
{
}

FTransform UBHGrabbableComponent::GetEquipRelativeTransform_Implementation()
{
	return FTransform::Identity;
}

void UBHGrabbableComponent::Throw_Implementation(const AActor* Instigator)
{
	Release_Implementation(Instigator);
}

