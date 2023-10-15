// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BHPlayerVocalComponent.generated.h"

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "PlayerCharacter", Meta = (BlueprintSpawnableComponent))
class STORMWATCH_API UBHPlayerCharacterVocalComponent : public UBHPlayerCharacterComponent
{
	GENERATED_BODY()

public:	
	UBHPlayerCharacterVocalComponent();

protected:
	virtual void BeginPlay() override;
};
