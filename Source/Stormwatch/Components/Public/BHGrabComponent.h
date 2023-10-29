// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "BHPhysicsInteractionComponent.h"
#include "BHGrabbableObjectInterface.h"
#include "BHGrabComponent.generated.h"

UCLASS(NotBlueprintable, BlueprintType, ClassGroup = "Interaction", Meta = (BlueprintSpawnableComponent))
class UBHGrabComponent : public UBHPhysicsInteractionComponent, public IBHGrabbableObject
{
	GENERATED_BODY()

public:
	//~ Begin BHGrabbableObject Interface.
	bool BeginGrab_Implementation(const AActor* Interactor) override;
	bool EndGrab_Implementation(const AActor* Interactor) override;
	EBHGrabType GetGrabType_Implementation() const override;
	//~ End BHGrabbableObject Interface.
};
