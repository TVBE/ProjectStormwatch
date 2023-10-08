// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "PlayerCharacterSceneComponent.generated.h"

UCLASS(Abstract)
class STORMWATCH_API UPlayerCharacterSceneComponent : public USceneComponent
{
	GENERATED_BODY()
	
public:
	UPlayerCharacterSceneComponent();

protected:
	virtual void OnRegister() override;
	virtual void OnUnregister() override;

	virtual void OnControllerChanged(APawn* Pawn, AController* NewController, AController* PreviousController) {};

	class APlayerCharacter* GetPlayerCharacter() const;
	class APlayerCharacterController* GetPlayerCharacterController() const;
};
