// Copyright (c) 2022-present Barrelhouse. All rights reserved.
// Written by Tim Verberne.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacterConfig.generated.h"

USTRUCT()
struct FPlayerSettings
{
	GENERATED_USTRUCT_BODY()

	virtual void InterpolateToTarget(const FPlayerSettings& Target, const float Time = 3.0f) {};
};

UCLASS(Abstract)
class STORMWATCH_API UPlayerCharacterConfigDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	virtual void Apply(APlayerCharacter* Character) = 0;
};
