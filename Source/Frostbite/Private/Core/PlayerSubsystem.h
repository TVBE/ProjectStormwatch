// Copyright 2023 Barrelhouse

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "PlayerSubsystem.generated.h"

class APlayerCharacter;
class APlayerController;

/** World Subsystem that provides access to the Player Character and its subobjects.
 *	Provides high level functions for changing the PlayerCharacter's behavior. */
UCLASS()
class UPlayerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

private:
	/** The Player Character in the world. */
	UPROPERTY()
	APlayerCharacter* PlayerCharacter {nullptr};

	/** The Player Controller for the Player Character. */
	UPROPERTY()
	APlayerCharacterController* PlayerController {nullptr};

public:
	/** Registers a Player Character to the subsystem.
	 *	@Character The PlayerCharacter to register.
	 */
	UFUNCTION(BlueprintCallable, Category = Player, Meta = (DisplayName = "Register Player Character"))
	void RegisterPlayerCharacter(APlayerCharacter* Character);

	/** Enables or disables movement input for the player. */
	UFUNCTION(BlueprintCallable, Category = Player, Meta = (DisplayName = "Set Player Movement Input Enabled"))
	void SetPlayerInputEnabled(const bool Value);

	/** Fades in the screen for the player from black by a specified amount.
	 *	@Duration The fade-in duration.
	 */
	UFUNCTION(BlueprintCallable, Category = Player, Meta = (DisplayName = "Fade From Black"))
	void FadePlayerCameraFromBlack(const float Duration);
	
	/** Returns the Player Character. */
	UFUNCTION(BlueprintPure, Category = Player, Meta = (DisplayName = "Get Player Character"))
	FORCEINLINE APlayerCharacter* GetPlayerCharacter() const {return PlayerCharacter; }

	/** Returns the Player Controller. */
	UFUNCTION(BlueprintPure, Category = Player, Meta = (DisplayName = "Get Player Controller"))
	FORCEINLINE APlayerCharacterController* GetPlayerController() const {return PlayerController; }
};
