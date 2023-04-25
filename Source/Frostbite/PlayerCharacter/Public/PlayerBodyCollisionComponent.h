// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "PlayerBodyCollisionComponent.generated.h"

UCLASS(NotBlueprintable, BlueprintType, ClassGroup = "PlayerCharacter", Within = "PlayerCharacter", Meta = (DisplayName = "Body Collision Component",
		ShortToolTip = "Collision component that pushes physics actors away from the player's body."))
class FROSTBITE_API UPlayerBodyCollisionComponent : public UCapsuleComponent
{
	GENERATED_BODY()

private:
	UPROPERTY()
	TArray<UPrimitiveComponent*> PushedComponents;

public:
	UPlayerBodyCollisionComponent();

protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;




};
