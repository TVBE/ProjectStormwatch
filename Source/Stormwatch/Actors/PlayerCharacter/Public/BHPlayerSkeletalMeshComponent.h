// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "Components/SkeletalMeshComponent.h"
#include "BHPlayerSkeletalMeshComponent.generated.h"

UCLASS(NotBlueprintable, BlueprintType, ClassGroup = "PlayerCharacter")
class STORMWATCH_API UBHPlayerSkeletalMeshComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()
	
public:
	UBHPlayerSkeletalMeshComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
