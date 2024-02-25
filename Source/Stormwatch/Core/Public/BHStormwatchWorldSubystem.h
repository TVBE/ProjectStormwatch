// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "BHStormwatchWorldSubystem.generated.h"

UCLASS(ClassGroup = "Barrelhouse")
class STORMWATCH_API UBHStormwatchWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	static UBHStormwatchWorldSubsystem* Get(const UObject* WorldContextObject);
	
	static void Register(class ABHPlayerCharacter* Character);
	static void Register(class ABHPlayerController* Controller);
	
	static void Unregister(const ABHPlayerCharacter* Character);
	static void Unregister(const ABHPlayerController* Controller);
	
	FORCEINLINE ABHPlayerCharacter* GetPlayerCharacter() const { return PlayerCharacter.Get(); }
	FORCEINLINE ABHPlayerController* GetPlayerController() const { return PlayerController.Get(); }

private:
	void SetPlayerCharacter(ABHPlayerCharacter* Character);
	void SetPlayerController(ABHPlayerController* Controller);
	
	TWeakObjectPtr<ABHPlayerCharacter> PlayerCharacter = nullptr;
	TWeakObjectPtr<ABHPlayerController> PlayerController = nullptr;
};
