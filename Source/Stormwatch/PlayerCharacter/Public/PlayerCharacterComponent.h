// Copyright (c) 2022-present Barrelhouse. All rights reserved.
// Written by Tim Verberne.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacterComponent.generated.h"

class APlayerCharacter;
class APlayerCharacterController;

UCLASS(Abstract)
class STORMWATCH_API UPlayerCharacterComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerCharacterComponent();

	virtual bool IsValid() const { return true; }

protected:
	virtual void OnRegister() override;

	virtual void BeginPlay() override;

	APlayerCharacter* GetPlayerCharacter() const;
	
	APlayerCharacterController* GetPlayerCharacterController() const;
};
