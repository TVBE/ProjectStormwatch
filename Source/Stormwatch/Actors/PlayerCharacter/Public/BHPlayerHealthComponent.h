// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BHPlayerAttributeComponent.h"
#include "BHPlayerHealthComponent.generated.h"

UCLASS(NotBlueprintable, BlueprintType, ClassGroup = "Barrelhouse")
class STORMWATCH_API UBHPlayerHealthComponent : public UBHPlayerAttributeComponent
{
	GENERATED_BODY()

public:
	UBHPlayerHealthComponent();
};
