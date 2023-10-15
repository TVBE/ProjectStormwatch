// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "BHInventoryObjectInterface.h"
#include "BHInventoryComponent.generated.h"

UCLASS(NotBlueprintable, BlueprintType, ClassGroup = "Interaction", Meta = (BlueprintSpawnableComponent))
class STORMWATCH_API UBHInventoryComponent : public UActorComponent, public IBHInventoryObject
{
	GENERATED_BODY()

public:
	//~ Begin BHInventoryObject Interface.
	bool AddToInventory_Implementation(const AActor* Actor);
	bool TakeFromInventory_Implementation(const AActor* Actor);
	//~ End BHInventoryObject Interface.
};
