// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHUseComponent.h"

UBHUseComponent::UBHUseComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UBHUseComponent::BeginUse_Implementation(const AActor* Interactor)
{
	OnBeginUse.Broadcast();
	return true;
}

bool UBHUseComponent::EndUse_Implementation(const AActor* Interactor)
{
	OnEndUse.Broadcast();
	return true;
}





