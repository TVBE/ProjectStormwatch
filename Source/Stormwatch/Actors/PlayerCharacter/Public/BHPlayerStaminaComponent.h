// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BHPlayerAttributeComponent.h"
#include "BHPlayerStaminaComponent.generated.h"

UCLASS(Blueprintable, BlueprintType, ClassGroup = "Barrelhouse", Meta = (BlueprintSpawnableComponent))
class STORMWATCH_API UBHPlayerStaminaComponent : public UBHPlayerAttributeComponent
{
	GENERATED_BODY()

public:
	UBHPlayerStaminaComponent();
};
