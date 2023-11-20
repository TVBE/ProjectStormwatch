// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "BHStormwatchWorldSubystem.generated.h"

UCLASS(ClassGroup = "Core")
class STORMWATCH_API UBHStormwatchWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogStormwatchWorldSubsystem, Log, All)

public:
	void RegisterPlayerCharacter(class ABHPlayerCharacter* Character);
	void RegisterPlayerController(class ABHPlayerController* Controller);
	
	void UnregisterPlayerCharacter(ABHPlayerCharacter* Character);
	void UnregisterPlayerController(ABHPlayerController* Controller);
	
	FORCEINLINE ABHPlayerCharacter* GetPlayerCharacter() const { return PlayerCharacter.Get(); }
	FORCEINLINE ABHPlayerController* GetPlayerController() const { return PlayerController.Get(); }

private:
	TWeakObjectPtr<ABHPlayerCharacter> PlayerCharacter = nullptr;
	TWeakObjectPtr<ABHPlayerController> PlayerController = nullptr;
	
	/** Integer value that is incremented or decremented when another object calls SetPlayerMovementInputLock.
	 *	If the value is zero, CanProcessMovementInput will be set to true for the player controller.*/
	uint8 MovementInputLockCount = 1;

	/** Integer value that is incremented or decremented when another object calls SetPlayerRotationInputLock.
	 *	If the value is zero, CanProcessRotationInput will be set to true for the player controller.*/
	uint8 RotationInputLockCount = 1;
};
