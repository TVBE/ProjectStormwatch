// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PlayerCharacterState.generated.h"

/**	This component is created to manage the state of the player character in the game.
 *	While Unreal Engine provides a default APlayerState class, we've opted to create an actor component instead.
 */
UCLASS()
class APlayerCharacterState : public APlayerState
{
	GENERATED_BODY()

private:
	/** The health value of the player character. If the player performs damaging actions, like falling from a great height, this value will be temporarily reduced.
	 *	The value will be increased back again over time. We use this value to play effects that resemble the player character being hurt.
	 */
	UPROPERTY()
	uint8 Health {100};
	
	/** The exertion of the player character. If the player performs physically intensive actions, such as jumping or sprinting, this value will be increased.
	 *	The value will be lowered when the player moves slowly. We use this value to play effects that resemble the player character becoming exhausted. 
	 */
	UPROPERTY()
	uint8 Exertion {0};

	/** The fear value of the player character. If the player encounters any of the hostile entities, this value will increase.
	 *	The value will be lowered when the player isn't near a hostile entity anymore. We use this value to play effects that resemble the player character being scared. 
	 */
	UPROPERTY()
	uint8 Fear {0};

	/** The vigilance value of the player character. This value is set according to certain in game events.
	 *	The value represents the player character being more alert to certain cues and sounds.
	 */
	UPROPERTY()
	uint8 Vigilance {0};

public:
	UFUNCTION(BlueprintCallable, Category = "PlayerState", Meta = (DisplayName = "Reset Player State"))
	void ResetPlayerState();
	
	UFUNCTION(BlueprintCallable, Category = "PlayerState|Health", Meta = (DisplayName = "Increment Health"))
	uint8 IncrementHealth(const uint8 Value) {IncrementValue(Health, Value); return Health; }
	
	UFUNCTION(BlueprintCallable, Category = "PlayerState|Health", Meta = (DisplayName = "Decrement Health"))
	uint8 DecrementHealth(const uint8 Value) {DecrementValue(Health, Value); return Health; }

	UFUNCTION(BlueprintCallable, Category = "PlayerState|Health", Meta = (DisplayName = "Set Health"))
	uint8 SetHealth(const uint8 Value) {SetValue(Health, Value); return Health; }

	UFUNCTION(BlueprintCallable, Category = "PlayerState|Exertion", Meta = (DisplayName = "Increment Exertion"))
	uint8 IncrementExertion(const uint8 Value) {IncrementValue(Exertion, Value); return Exertion; }
	
	UFUNCTION(BlueprintCallable, Category = "PlayerState|Exertion", Meta = (DisplayName = "Decrement Exertion"))
	uint8 DecrementExertion(const uint8 Value) {DecrementValue(Exertion, Value); return Exertion; }

	UFUNCTION(BlueprintCallable, Category = "PlayerState|Exertion", Meta = (DisplayName = "Set Exertion"))
	uint8 SetExertion(const uint8 Value) {SetValue(Exertion, Value); return Exertion; }

	UFUNCTION(BlueprintCallable, Category = "PlayerState|Fear", Meta = (DisplayName = "Increment Fear"))
	uint8 IncrementFear(const uint8 Value) {IncrementValue(Fear, Value); return Fear; }
	
	UFUNCTION(BlueprintCallable, Category = "PlayerState|Fear", Meta = (DisplayName = "Decrement Fear"))
	uint8 DecrementFear(const uint8 Value) {DecrementValue(Fear, Value); return Fear; }

	UFUNCTION(BlueprintCallable, Category = "PlayerState|Fear", Meta = (DisplayName = "Set Fear"))
	uint8 SetFear(const uint8 Value) {SetValue(Fear, Value); return Fear; }

	UFUNCTION(BlueprintCallable, Category = "PlayerState|Vigilance", Meta = (DisplayName = "Increment Vigilance"))
	uint8 IncrementVigilance(const uint8 Value) {IncrementValue(Vigilance, Value); return Vigilance; }
	
	UFUNCTION(BlueprintCallable, Category = "PlayerState|Vigilance", Meta = (DisplayName = "Decrement Vigilance"))
	uint8 DecrementVigilance(const uint8 Value) {DecrementValue(Vigilance, Value); return Vigilance; }

	UFUNCTION(BlueprintCallable, Category = "PlayerState|Vigilance", Meta = (DisplayName = "Set Vigilance"))
	uint8 SetVigilance(const uint8 Value) {SetValue(Vigilance, Value); return Vigilance; }

private:
	uint8 IncrementValue(uint8& Property, uint8 Value);
	
	uint8 DecrementValue(uint8& Property, uint8 Value);

	uint8 SetValue(uint8& Property, uint8 Value);

public:
	UFUNCTION(BlueprintPure, Category = "PlayerState|Health", Meta = (DisplayName = "Get Health"))
	FORCEINLINE uint8 GetHealth() const {return Health; }

	UFUNCTION(BlueprintPure, Category = "PlayerState|Exertion", Meta = (DisplayName = "Get Exertion"))
	FORCEINLINE uint8 GetExertion() const {return Exertion; }

	UFUNCTION(BlueprintPure, Category = "PlayerState|Fear", Meta = (DisplayName = "Get Fear"))
	FORCEINLINE uint8 GetFear() const {return Fear; }

	UFUNCTION(BlueprintPure, Category = "PlayerState|Vigilance", Meta = (DisplayName = "Get Vigilance"))
	FORCEINLINE uint8 GetVigilance() const {return Vigilance; }
};
