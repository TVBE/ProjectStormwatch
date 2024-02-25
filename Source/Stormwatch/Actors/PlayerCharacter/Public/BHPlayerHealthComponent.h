// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BHPlayerAttributeComponent.h"
#include "BHPlayerHealthComponent.generated.h"

UCLASS(Blueprintable, BlueprintType, ClassGroup = "Barrelhouse", Meta = (BlueprintSpawnableComponent))
class STORMWATCH_API UBHPlayerHealthComponent : public UBHPlayerAttributeComponent
{
	GENERATED_BODY()

public:
	UBHPlayerHealthComponent();
};
