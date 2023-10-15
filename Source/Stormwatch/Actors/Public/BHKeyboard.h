// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GrabbableObjectInterface.h"
#include "GameFramework/Actor.h"
#include "BHKeyboard.generated.h"

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "Desktop")
class STORMWATCH_API ABHKeyboard : public AActor, public IGrabbableObject
{
	GENERATED_BODY()

public:	
	ABHKeyboard();

public:
	/** IInteractableObject interface functions. */
	FORCEINLINE bool BeginGrab_Implementation(const AActor* Interactor) override { return false; }
	FORCEINLINE bool EndGrab_Implementation(const AActor* Interactor) override { return false; }
	FORCEINLINE EGrabType GetGrabType_Implementation() const override { return EGrabType::OneHanded; }

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure)
	FString GetRandomCharacter();
};
