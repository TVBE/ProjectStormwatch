// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BHBlizzard.generated.h"

/** Actor that handles blizzard VFX around the player. */
UCLASS(Abstract, Blueprintable, BlueprintType)
class ABHBlizzard : public AActor
{
	GENERATED_BODY()
	
public:	
	ABHBlizzard();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
};
