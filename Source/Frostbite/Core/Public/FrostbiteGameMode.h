// Copyright 2023 Barrelhouse

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FrostbiteGameMode.generated.h"

class APlayerCharacter;

/**
 * 
 */
UCLASS()
class AFrostbiteGameMode : public AGameModeBase
{
	GENERATED_BODY()

private:
	UPROPERTY()
	bool IsPlayerActive {false};

public:
	/** Notifies the gamemode that a player character is fully initialized and is ready for use. */
	void NotifyPlayerCharacterBeginPlay(APlayerCharacter* Character);

protected:
	/** Called when the player character is ready for use in the world. */
	UFUNCTION(BlueprintNativeEvent, Category = Default, Meta = (DisplayName = "On Player Spawn"))
	void OnPlayerSpawn(APlayerCharacter* Character);
	
};
