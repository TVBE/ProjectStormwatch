// Copyright (c) 2022-present Barrelhouse. All rights reserved.

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

protected:
	virtual void OnRegister() override;
	virtual void OnUnregister() override;

	virtual void OnControllerChanged(APawn* Pawn, AController* NewController, AController* PreviousController) {};

	class APlayerCharacter* GetPlayerCharacter() const;
	class APlayerCharacterController* GetPlayerCharacterController() const;
};
