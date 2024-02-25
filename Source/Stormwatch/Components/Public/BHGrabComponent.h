// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "BHPhysicsInteractableComponent.h"
#include "BHGrabbableObjectInterface.h"
#include "BHGrabComponent.generated.h"

UCLASS(NotBlueprintable, BlueprintType, ClassGroup = "Barrelhouse", Meta = (BlueprintSpawnableComponent))
class UBHGrabComponent : public UBHPhysicsInteractableComponent, public IBHGrabbableObject
{
	GENERATED_BODY()

public:
	UBHGrabComponent();
	
	//~ Begin BHGrabbableObject Interface.
	bool BeginGrab_Implementation(const AActor* Interactor) override;
	bool EndGrab_Implementation(const AActor* Interactor) override;
	EBHGrabType GetGrabType_Implementation() const override;
	//~ End BHGrabbableObject Interface.
};
