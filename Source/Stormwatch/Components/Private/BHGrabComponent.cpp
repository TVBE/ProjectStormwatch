// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHGrabComponent.h"

UBHGrabComponent::UBHGrabComponent()
{
#if WITH_EDITOR
	bAllowOnlyOneInstancePerActor = true;
#endif
}

bool UBHGrabComponent::BeginGrab_Implementation(const AActor* Interactor)
{
	HandleOnInteractionStart();
	return true;
}

bool UBHGrabComponent::EndGrab_Implementation(const AActor* Interactor)
{
	HandleOnInteractionEnd();
	return true;
}

EBHGrabType UBHGrabComponent::GetGrabType_Implementation() const
{
	return EBHGrabType::OneHanded;
}