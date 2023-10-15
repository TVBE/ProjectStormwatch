// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "BHInteractionComponent.h"
#include "BHGrabbableObjectInterface.h"
#include "BHInventoryObjectInterface.h"
#include "BHGrabComponent.generated.h"

UCLASS(NotBlueprintable, BlueprintType, ClassGroup = "Interaction", Meta = (BlueprintSpawnableComponent))
class UBHGrabComponent : public UBHInteractionComponent, public IBHGrabbableObject
{
	GENERATED_BODY()

public:
	//~ Begin BHGrabbableObject Interface.
	bool BeginGrab_Implementation(const AActor* Interactor) override { return false; }
	bool EndGrab_Implementation(const AActor* Interactor) override { return false; }
	EBHGrabType GetGrabType_Implementation() const override { return EBHGrabType::OneHanded; }
	//~ End BHGrabbableObject Interface.
};
