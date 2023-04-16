// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "GrabbableObjectInterface.h"
#include "GameFramework/Actor.h"
#include "Keyboard.generated.h"

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "Interaction", Meta = (DisplayName = "Keyboard"))
class FROSTBITE_API AKeyboard : public AActor, public IGrabbableObject
{
	GENERATED_BODY()

public:	
	AKeyboard();

public:
	/** IInteractableObject interface functions. */
	FORCEINLINE bool BeginGrab_Implementation(const AActor* Interactor) override { return false; }
	FORCEINLINE bool EndGrab_Implementation(const AActor* Interactor) override { return false; }
	FORCEINLINE EGrabType GetGrabType_Implementation() const override { return EGrabType::OneHanded; }

protected:
	virtual void BeginPlay() override;

	/** Returns a random character. */
	UFUNCTION(BlueprintPure, Category = "Keyboard", Meta = (DisplayName = "Get Random Character"))
	FString GetRandomCharacter();
};
