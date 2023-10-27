// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BHNightstalkerAudioComponent.generated.h"


UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "Nightstalker", Meta = (BlueprintSpawnableComponent))
class UBHNightstalkerAudioComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBHNightstalkerAudioComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
protected:
	virtual void BeginPlay() override;
};
