// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "BHInventoryObjectInterface.h"
#include "BHPhysicsMeshActor.h"
#include "BHGrabbableMeshActor.generated.h"

// A mesh actor that can be grabbed by the player.
UCLASS(Blueprintable, BlueprintType, ClassGroup = "Barrelhouse")
class STORMWATCH_API ABHGrabbableMeshActor : public ABHPhysicsMeshActor, public IBHInventoryObject
{
	GENERATED_BODY()

public:
	ABHGrabbableMeshActor();

	// IBHInventoryObject implementation begin
	virtual bool AddToInventory_Implementation() override;
	virtual bool RemoveFromInventory_Implementation() override;
	virtual bool CanAddToInventory_Implementation() const override;
	// IBHInventoryObject implementation end
	
	UFUNCTION(BlueprintGetter)
	class UBHGrabComponent* GetGrabComponent() const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	bool bCanBeAddedToInventory = false;

private:
	UPROPERTY(EditAnywhere, BlueprintGetter = GetGrabComponent, Category = "Components")
	UBHGrabComponent* GrabComponent;
};
