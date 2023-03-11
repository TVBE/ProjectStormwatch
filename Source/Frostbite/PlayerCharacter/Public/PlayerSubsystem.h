// Copyright 2023 Barrelhouse

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "PlayerSubsystem.generated.h"

class APlayerCharacter;
class APlayerCharacterController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerMovementInputLockDelegate, bool, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerRotationInputLockDelegate, bool, Value);

/** World Subsystem that provides access to the Player Character and its subobjects.
 *	Provides high level functions for changing the PlayerCharacter's behavior. */
UCLASS()
class UPlayerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	/** Delegate for when the player controller should start or stop processing player movement input. */
	UPROPERTY(BlueprintAssignable, Category = "Player|Input", Meta = (DisplayName = "On Movement Input Lock Changed"))
	FPlayerMovementInputLockDelegate OnMovementInputLockChanged;

	/** Delegate for when the player controller should start or stop processing player rotation input. */
	UPROPERTY(BlueprintAssignable, Category = "Player|Input", Meta = (DisplayName = "On Rotation Input Lock Changed"))
	FPlayerRotationInputLockDelegate OnRotationInputLockChanged;
	
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

	/** Adds or removes a movement input lock for the player controller. The player controller can only process movement input if there are no locks present.
	 *	@Value Whether a lock should be added or removed.
	 *	@Return If the player can now process movement input or not. This will only be the case if there are zero locks present.
	 */
	UFUNCTION(BlueprintCallable, Category = "Player|Input", Meta = (DisplayName = "Set Player Movement Input Lock"))
	bool SetPlayerMovementInputLock(const bool Value);

	/** Adds or removes a movement input lock for the player controller. The player controller can only process movement input if there are no locks present.
	*	@Value Whether a lock should be added or removed.
	*	@Return If the player can now process movement input or not. This will only be the case if there are zero locks present.
	*/
	UFUNCTION(BlueprintCallable, Category = "Player|Input", Meta = (DisplayName = "Set Player Movement Rotation Lock"))
	bool SetPlayerRotationInputLock(const bool Value);
	
	/** Fades in the screen for the player from black by a specified amount.
	 *	@Duration The fade-in duration.
	 */
	UFUNCTION(BlueprintCallable, Category = "Player", Meta = (DisplayName = "Fade From Black"))
	void FadePlayerCameraFromBlack(const float Duration);
	
	/** Returns the Player Character. */
	UFUNCTION(BlueprintPure, Category = "Player", Meta = (DisplayName = "Get Player Character"))
	FORCEINLINE APlayerCharacter* GetPlayerCharacter() const {return PlayerCharacter; }

	/** Returns the Player Controller. */
	UFUNCTION(BlueprintPure, Category = "Player", Meta = (DisplayName = "Get Player Controller"))
	FORCEINLINE APlayerCharacterController* GetPlayerController() const {return PlayerController; }
};
