// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "Nightstalker.generated.h"

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = (Nightstalker))
class ANightstalker : public APawn
{
	GENERATED_BODY()
	
public:
	ANightstalker();
protected:
	/** The CapsuleComponent being used for collision.*/
	UPROPERTY(Category="Collision", VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UCapsuleComponent> CapsuleComponent;
	
	virtual void BeginPlay() override;
	
public:
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
