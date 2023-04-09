// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "InteractableObjectInterface.h"
#include "GameFramework/Actor.h"
#include "Keyboard.generated.h"

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "Interaction", Meta = (DisplayName = "Keyboard"))
class FROSTBITE_API AKeyboard : public AActor, public IInteractableObject
{
	GENERATED_BODY()
	
public:	
	AKeyboard();

public:
	/** IInteractableObject interface functions. */
	FORCEINLINE bool Use_Implementation(const AActor* Interactor) override { return false; }
	FORCEINLINE bool Disuse_Implementation(const AActor* Interactor) override { return false; }
	FORCEINLINE EInteractionType GetInteractionType_Implementation() const override { return EInteractionType::Grabbable; }
	FORCEINLINE EInteractionTriggerType GetInteractionTriggerType_Implementation() const override { return EInteractionTriggerType::SinglePress; }
	FORCEINLINE EInteractionHandType GetInteractionHandType_Implementation() const override { return EInteractionHandType::OneHanded; }
	FORCEINLINE FVector GetInteractionWidgetOffset_Implementation() const override { return FVector(); }

protected:
	virtual void BeginPlay() override;

	/** Returns a random character. */
	UFUNCTION(BlueprintPure, Category = "Keyboard", Meta = (DisplayName = "Get Random Character"))
	FString GetRandomCharacter();
};
