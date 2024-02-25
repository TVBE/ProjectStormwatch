// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "BHPlayerCharacterComponent.h"
#include "BHPlayerVocalComponent.generated.h"

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "BHPlayerCharacter", Meta = (BlueprintSpawnableComponent))
class STORMWATCH_API UBHPlayerCharacterVocalComponent : public UBHPlayerCharacterComponent_Deprecated
{
	GENERATED_BODY()

public:	
	UBHPlayerCharacterVocalComponent();

protected:
	virtual void BeginPlay() override;
};
