// Copyright (c) 2022-present Barrelhouse. All rights reserved.
// Written by Tim Verberne.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NightstalkerVfxController.generated.h"


UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = (Nightstalker), Meta = (BlueprintSpawnableComponent) )
class STORMWATCH_API UNightstalkerVfxController : public UActorComponent
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
