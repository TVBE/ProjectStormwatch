// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BHGrabbableObjectInterface.generated.h"

UENUM(BlueprintType)
enum class EBHGrabType : uint8
{
	OneHanded			UMETA(DisplayName = "One Handed"),
	TwoHanded			UMETA(DisplayName = "Two Handed"),
};

UINTERFACE(Blueprintable)
class STORMWATCH_API UBHGrabbableObject : public UInterface
{
	GENERATED_BODY()
};

class STORMWATCH_API IBHGrabbableObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grabbable Object")
	bool Grab(const AActor* Interactor);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grabbable Object")
	bool Release(const AActor* Interactor);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grabbable Object")
	EBHGrabType GetGrabType() const;
};