// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "Camera/CameraComponent.h"
#include "BHPlayerCameraComponent.generated.h"

UCLASS(NotBlueprintable, BlueprintType, ClassGroup = "PlayerCharacter")
class STORMWATCH_API UBHPlayerCameraComponent : public UCameraComponent
{
	GENERATED_BODY()
	
public:
	UBHPlayerCameraComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	
};
