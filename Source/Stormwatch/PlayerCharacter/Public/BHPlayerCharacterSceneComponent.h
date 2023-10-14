// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "BHPlayerCharacterSceneComponent.generated.h"

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "PlayerCharacter")
class STORMWATCH_API UBHPlayerCharacterSceneComponent : public USceneComponent
{
	GENERATED_BODY()
	
public:
	UBHPlayerCharacterSceneComponent();

protected:
	virtual void OnRegister() override;
	virtual void OnUnregister() override;

	virtual void OnControllerChanged(APawn* Pawn, AController* NewController, AController* PreviousController) {};

	class ABHPlayerCharacter* GetPlayerCharacter() const;
	class ABHPlayerCharacterController* GetPlayerCharacterController() const;
};
