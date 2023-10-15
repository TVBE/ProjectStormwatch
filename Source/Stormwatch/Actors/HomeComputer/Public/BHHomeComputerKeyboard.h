// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "..\..\..\Interfaces\Public\BHGrabbableObjectInterface.h"
#include "GameFramework/Actor.h"
#include "BHHomeComputerKeyboard.generated.h"

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "Desktop")
class STORMWATCH_API ABHHomeComputerKeyboard : public AActor, public IBHGrabbableObject
{
	GENERATED_BODY()

public:	
	ABHHomeComputerKeyboard();

public:
	/** IInteractableObject interface functions. */
	FORCEINLINE bool BeginGrab_Implementation(const AActor* Interactor) override { return false; }
	FORCEINLINE bool EndGrab_Implementation(const AActor* Interactor) override { return false; }
	FORCEINLINE EBHGrabType GetGrabType_Implementation() const override { return EBHGrabType::OneHanded; }

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure)
	FString GetRandomCharacter();
};
