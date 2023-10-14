// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "StormwatchWorldSubystem.generated.h"

class ANightstalker;
class ABHPlayerCharacter;
class ABHPlayerCharacterController;

/** World Subsystem that provides access to the Player Character and its subobjects.
 *	Provides high level functions for changing the PlayerCharacter's behavior. */
UCLASS(ClassGroup = "Core", Meta = (DisplayName = "Stormwatch World Subsystem"))
class STORMWATCH_API UStormwatchWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogStormwatchWorldSubsystem, Log, All)

private:
	/** The Player Character in the world. */
	UPROPERTY()
	ABHPlayerCharacter* PlayerCharacter = nullptr;

	/** The Player Controller for the Player Character. */
	UPROPERTY()
	ABHPlayerCharacterController* PlayerController = nullptr;
	
	/** Integer value that is incremented or decremented when another object calls SetPlayerMovementInputLock.
	 *	If the value is zero, CanProcessMovementInput will be set to true for the player controller.*/
	uint8 MovementInputLockCount = 1;

	/** Integer value that is incremented or decremented when another object calls SetPlayerRotationInputLock.
	 *	If the value is zero, CanProcessRotationInput will be set to true for the player controller.*/
	uint8 RotationInputLockCount = 1;

public:
	/** Registers a Player Character to the subsystem.
	 *	@Character The PlayerCharacter to register. */
	UFUNCTION(BlueprintCallable, Category = "Player")
	void RegisterPlayerCharacter(ABHPlayerCharacter* Character);

	/** Registers a Player Controller to the subsystem.
	*	@Controller The PlayerController to register. */
	UFUNCTION(BlueprintCallable, Category = "Player")
	void RegisterPlayerController(ABHPlayerCharacterController* Controller);

	/** Unregisters a Player Character from the subsystem. This will be ignored if the player character is not already registered.
	*	@Character The PlayerCharacter to unregister. */
	UFUNCTION(BlueprintCallable, Category = "Player")
	void UnregisterPlayerCharacter(ABHPlayerCharacter* Character);
	
	/** Unregisters a Player Controller from the subsystem. This will be ignored if the player controller is not already registered.
	*	@Controller The PlayerController to unregister. */
	UFUNCTION(BlueprintCallable, Category = "Player")
	void UnregisterPlayerController(ABHPlayerCharacterController* Controller);
	
	/** Returns the Player Character. */
	UFUNCTION(BlueprintPure, Category = "Player")
	FORCEINLINE ABHPlayerCharacter* GetPlayerCharacter() const { return PlayerCharacter; }

	/** Returns the Player Controller. */
	UFUNCTION(BlueprintPure, Category = "Player")
	FORCEINLINE ABHPlayerCharacterController* GetPlayerController() const { return PlayerController; }
};
