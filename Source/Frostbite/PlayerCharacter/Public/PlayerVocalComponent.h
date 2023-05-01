// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerVocalComponent.generated.h"

/** Actor component that handles all vocal audio for the player character. */
UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "PlayerCharacter", Meta = (BlueprintSpawnableComponent,
	DisplayName = "Player Vocal Component", ShortToolTip = "Component that handles vocal audio for the player character."))
class FROSTBITE_API UPlayerCharacterVocalComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPlayerCharacterVocalComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
