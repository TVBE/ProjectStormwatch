// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "StormwatchGameMode.generated.h"

class ABHPlayerCharacter;

/**
 * 
 */
UCLASS()
class AStormwatchGameMode : public AGameModeBase
{
	GENERATED_BODY()

private:
	UPROPERTY()
	bool IsPlayerActive = false;

public:
	AStormwatchGameMode();
	
	/** Notifies the gamemode that a player character is fully initialized and is ready for use. */
	void NotifyPlayerCharacterBeginPlay(ABHPlayerCharacter* Character);

protected:
	/** Called when the player character is ready for use in the world. */
	UFUNCTION(BlueprintNativeEvent, Category = "Events", Meta = (DisplayName = "On Player Spawn"))
	void EventOnPlayerSpawn(ABHPlayerCharacter* Character);
};
