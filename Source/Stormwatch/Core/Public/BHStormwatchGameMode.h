// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BHStormwatchGameMode.generated.h"

UCLASS()
class ABHStormwatchGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABHStormwatchGameMode();
	
	void NotifyPlayerCharacterBeginPlay(class ABHPlayerCharacter* Character);

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Events", Meta = (DisplayName = "On Player Spawn"))
	void EventOnPlayerSpawn(ABHPlayerCharacter* Character);

private:
	UPROPERTY()
	bool IsPlayerActive = false;
};
