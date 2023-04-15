// // Copyright (c) 2022-present Barrelhouse// Written by // This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PowerConsumerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UPowerConsumerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPowerConsumerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
