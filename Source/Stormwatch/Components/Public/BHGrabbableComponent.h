// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "BHPhysicsInteractableComponent.h"
#include "BHGrabbableObjectInterface.h"
#include "BHGrabbableComponent.generated.h"

UCLASS(Blueprintable, BlueprintType, ClassGroup = "Barrelhouse", Meta = (BlueprintSpawnableComponent))
class UBHGrabbableComponent : public UBHPhysicsInteractableComponent, public IBHGrabbableObject
{
	GENERATED_BODY()

public:
	UBHGrabbableComponent();
	
	//~ Begin BHGrabbableObject Interface.
	virtual bool Grab_Implementation(const AActor* Instigator) override;
	virtual void Release_Implementation(const AActor* Instigator) override;
	virtual void Equip_Implementation(const AActor* Instigator) override;
	virtual void Unequip_Implementation(const AActor* Instigator) override;
	virtual FTransform GetEquipRelativeTransform_Implementation() override;
	virtual void Throw_Implementation(const AActor* Instigator) override;
	//~ End BHGrabbableObject Interface.
};
