// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NightstalkerVfxController.generated.h"


UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = (Nightstalker), Meta = (BlueprintSpawnableComponent) )
class UNightstalkerVfxController : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UNightstalkerVfxController();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
