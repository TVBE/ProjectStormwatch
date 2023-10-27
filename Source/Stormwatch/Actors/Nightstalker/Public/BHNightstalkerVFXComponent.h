// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BHNightstalkerVFXComponent.generated.h"

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "Nightstalker", Meta = (BlueprintSpawnableComponent) )
class STORMWATCH_API UBHNightstalkerVFXComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBHNightstalkerVFXComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;
};
