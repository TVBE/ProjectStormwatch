// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerVocalComponent.generated.h"

/** Actor component that handles all vocal audio for the player character. */
UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = (PlayerCharacter), Meta = (BlueprintSpawnableComponent))
class FROSTBITE_API UPlayerCharacterVocalComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerCharacterVocalComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
