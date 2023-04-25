// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "FrostbiteWorldSubystem.generated.h"

class APlayerCharacter;
class APlayerCharacterController;

/** World Subsystem that provides access to the Player Character and its subobjects.
 *	Provides high level functions for changing the PlayerCharacter's behavior. */
UCLASS(ClassGroup = "Core", Meta = (DisplayName = "Frostbite World Subsystem"))
class FROSTBITE_API UFrostbiteWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogFrostbiteWorldSubsystem, Log, All)

private:
	/** The Player Character in the world. */
	UPROPERTY()
	APlayerCharacter* PlayerCharacter {nullptr};

	/** The Player Controller for the Player Character. */
	UPROPERTY()
	APlayerCharacterController* PlayerController {nullptr};

	/** Integer value that is incremented or decremented when another object calls SetPlayerMovementInputLock.
	 *	If the value is zero, CanProcessMovementInput will be set to true for the player controller.*/
	uint8 MovementInputLockCount {1};

	/** Integer value that is incremented or decremented when another object calls SetPlayerRotationInputLock.
	 *	If the value is zero, CanProcessRotationInput will be set to true for the player controller.*/
	uint8 RotationInputLockCount {1};

public:
	/** Registers a Player Character to the subsystem.
	 *	@Character The PlayerCharacter to register.
	 */
	UFUNCTION(BlueprintCallable, Category = "Player", Meta = (DisplayName = "Register Player Character"))
	void RegisterPlayerCharacter(APlayerCharacter* Character);

	/** Registers a Player Controller to the subsystem.
	*	@Controller The PlayerController to register.
	*/
	UFUNCTION(BlueprintCallable, Category = "Player", Meta = (DisplayName = "Register Player Controller"))
	void RegisterPlayerController(APlayerCharacterController* Controller);

	/** Unregisters a Player Character from the subsystem. This will be ignored if the player character is not already registered.
	*	@Character The PlayerCharacter to unregister.
	*/
	UFUNCTION(BlueprintCallable, Category = "Player", Meta = (DisplayName = "Unregister Player Character"))
	void UnregisterPlayerCharacter(APlayerCharacter* Character);
	
	/** Unregisters a Player Controller from the subsystem. This will be ignored if the player controller is not already registered.
	*	@Controller The PlayerController to unregister.
	*/
	UFUNCTION(BlueprintCallable, Category = "Player", Meta = (DisplayName = "Unregister Player Controller"))
	void UnregisterPlayerController(APlayerCharacterController* Controller);
	
	/** Returns the Player Character. */
	UFUNCTION(BlueprintPure, Category = "Player", Meta = (DisplayName = "Get Player Character"))
	FORCEINLINE APlayerCharacter* GetPlayerCharacter() const { return PlayerCharacter; }

	/** Returns the Player Controller. */
	UFUNCTION(BlueprintPure, Category = "Player", Meta = (DisplayName = "Get Player Controller"))
	FORCEINLINE APlayerCharacterController* GetPlayerController() const { return PlayerController; }
};