// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BHGrabbableObjectInterface.generated.h"

UINTERFACE(Blueprintable)
class STORMWATCH_API UBHGrabbableObject : public UInterface
{
	GENERATED_BODY()
};

class STORMWATCH_API IBHGrabbableObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BHGrabbableObject")
	bool Grab(const AActor* Instigator);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BHGrabbableObject")
	void Release(const AActor* Instigator);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BHGrabbableObject")
	void Equip(const AActor* Instigator);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BHGrabbableObject")
	void Unequip(const AActor* Instigator);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BHGrabbableObject")
	const FTransform& GetEquipRelativeTransform();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BHGrabbableObject")
	void Throw(const AActor* Instigator);
};