// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BHPlayerCharacterComponent.h"
#include "Components/CapsuleComponent.h"
#include "BHPlayerBodyCollisionComponent.generated.h"

UCLASS(NotBlueprintable, BlueprintType, ClassGroup = "Barrelhouse")
class STORMWATCH_API UBHPlayerBodyCollisionComponent : public UCapsuleComponent, public FBHPlayerCharacterComponent
{
	GENERATED_BODY()

public:
	UBHPlayerBodyCollisionComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;
	virtual void OnRegister() override;
	virtual void OnUnregister() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Collision")
	TArray<TEnumAsByte<ECollisionChannel>> BodyCollisionChannels {ECC_GameTraceChannel2};
	
	UPROPERTY(Transient)
	TArray<UPrimitiveComponent*> PushedComponents;
};
