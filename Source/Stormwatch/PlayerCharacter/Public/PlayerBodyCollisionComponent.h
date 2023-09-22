// Copyright (c) 2022-present Barrelhouse. All rights reserved.
// Written by Tim Verberne.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "PlayerBodyCollisionComponent.generated.h"

UCLASS(NotBlueprintable, BlueprintType, ClassGroup = "PlayerCharacter", Within = "PlayerCharacter",
	   Meta = (DisplayName = "Body Collision Component", ShortToolTip = "Collision component that pushes physics actors away from the player's body."))
class STORMWATCH_API UPlayerBodyCollisionComponent : public UCapsuleComponent
{
	GENERATED_BODY()

public:
	UPlayerBodyCollisionComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void OnRegister() override;

	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TArray<UPrimitiveComponent*> PushedComponents;
};
