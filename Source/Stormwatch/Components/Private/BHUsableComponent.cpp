// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHUsableComponent.h"

UBHUsableComponent::UBHUsableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UBHUsableComponent::Use_Implementation(const AActor* Instigator)
{
	BeginInteraction();
	OnBeginUse.Broadcast();
	return true;
}

void UBHUsableComponent::Release_Implementation(const AActor* Instigator)
{
	EndInteraction();
	OnEndUse.Broadcast();
}

EBHUseType UBHUsableComponent::GetUseType_Implementation() const
{
	return UseType;
}