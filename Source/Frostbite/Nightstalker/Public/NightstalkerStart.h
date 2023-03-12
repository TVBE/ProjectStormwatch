// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NightstalkerStart.generated.h"

UCLASS(NotBlueprintable, BlueprintType, Placeable, ClassGroup = (Nightstalker))
class ANightstalkerStart : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANightstalkerStart();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
