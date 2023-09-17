// Copyright (c) 2022-present Barrelhouse. All rights reserved.
// Written by Tim Verberne.

#pragma once

#include "CoreMinimal.h"
#include "StormwatchMacros.h"
#include "PlayerCharacter.h"
#include "PlayerStateComponent.generated.h"

class UPlayerStateConfiguration;

UENUM(BlueprintType)
enum class EPlayerStateValue : uint8
{
	Pain				UMETA(DisplayName = "Pain"),
	Exertion			UMETA(DisplayName = "Exertion"),
	Fear				UMETA(DisplayName = "Fear"),
	Vigilence			UMETA(DisplayName = "Vigilance"),
};

USTRUCT(BlueprintType)
struct FPlayerStateSettings
{
	GENERATED_USTRUCT_BODY()

	/** The amount of pain that is restored every update interval. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Pain", Meta = (DisplayName = "Pain Reduction Amount"))
	float PainReductionAmount {2.0f};

	/** The amount of exertion that is removed per update interval when the player is not performing intensive movements.*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Exertion", Meta = (DisplayName = "Exertion Reduction Amount"))
	float ExertionReductionAmount {2.0f};

	/** The amount of exertion that is added per update interval when the player sprints. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Exertion", Meta = (DisplayName = "Sprint Exertion Cost"))
	float SprintExertionCost {3.0f};

	/** The amount of exertion that is added when the player jumps. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Exertion", Meta = (DisplayName = "Jump Exertion Cost"))
	float JumpExertionCost {5.0f};
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPainChangedDelegate, const float, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnExertionChangedDelegate, const float, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFearChangedDelegate, const float, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVigilanceChangedDelegate, const float, Value);

/**	This component is created to manage the state of the player character in the game.
 *	While Unreal Engine provides a default APlayerState class, we've opted to create an actor component instead.
 */
UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "PlayerCharacter", Meta = (BlueprintSpawnableComponent,
	DisplayName = "Player State Component", ShortToolTip = "Component that manages the player's state."))
class STORMWATCH_API UPlayerStateComponent : public UActorComponent
{
	GENERATED_BODY()

	PLAYER_COMPONENT_BODY()

public:
	/** Called when the Pain value is changed. */
	UPROPERTY(BlueprintAssignable, Category = "PlayerState|Delegates", Meta = (DisplayName = "On Pain Changed"))
	FOnPainChangedDelegate OnPainChanged;

	/** Called when the exertion value is changed. */
	UPROPERTY(BlueprintAssignable, Category = "PlayerState|Delegates", Meta = (DisplayName = "On Exertion Changed"))
	FOnExertionChangedDelegate OnExertionChanged;

	/** Called when the fear value is changed. */
	UPROPERTY(BlueprintAssignable, Category = "PlayerState|Delegates", Meta = (DisplayName = "On Fear Changed"))
	FOnFearChangedDelegate OnFearChanged;

	/** Called when the vigilance value is changed. */
	UPROPERTY(BlueprintAssignable, Category = "PlayerState|Delegates", Meta = (DisplayName = "On Vigilance Changed"))
	FOnVigilanceChangedDelegate OnVigilanceChanged;

private:
	/** The configuration asset to use for this playerstate component. */
	UPROPERTY(EditAnywhere, Category = "PlayerState|Configuration", Meta = (DisplayName = "Configuration Asset", DisplayPriority = "0"))
	FPlayerStateSettings Settings;

	/** Pointer to the player state configuration. */
	UPROPERTY(BlueprintGetter = GetConfiguration)
	UPlayerStateConfiguration* Configuration;

	/** The pain value of the player character. If the player performs damaging actions, like falling from a great height, this value will be temporarily reduced.
	 *	The value will be increased back again over time. We use this value to play effects that resemble the player character being hurt. */
	UPROPERTY()
	float Pain {0.0f};

	/** The exertion of the player character. If the player performs physically intensive actions, such as jumping or sprinting, this value will be increased.
	 *	The value will be lowered when the player moves slowly. We use this value to play effects that resemble the player character becoming exhausted. */
	UPROPERTY()
	float Exertion {0.0f};

	/** The fear value of the player character. If the player encounters any of the hostile entities, this value will increase.
	 *	The value will be lowered when the player isn't near a hostile entity anymore. We use this value to play effects that resemble the player character being scared. */
	UPROPERTY()
	float Fear {0.0f};

	/** The vigilance value of the player character. This value is set according to certain in game events.
	 *	The value represents the player character being more alert to certain cues and sounds. */
	UPROPERTY()
	float Vigilance {0.0f};

	/** Timer handle for the state update timer. */
	UPROPERTY()
	FTimerHandle StateTimer;

public:
	UFUNCTION(BlueprintCallable, Category = "PlayerState", Meta = (DisplayName = "Reset Player State"))
	void ResetPlayerState();

private:
	virtual void OnComponentCreated() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	float IncrementValue(float& Property, float Value);
	float DecrementValue(float& Property, float Value);
	float SetValue(float& Property, float Value);

	/** Updates the player character state. */
	UFUNCTION()
	void UpdatePlayerState();

protected:
	UFUNCTION(BlueprintCallable, Category = "PlayerState", Meta = (DisplayName = "Increment Value", BlueprintProtected))
	float IncrementStateValue(const EPlayerStateValue Type, const float Value);

	UFUNCTION(BlueprintCallable, Category = "PlayerState", Meta = (DisplayName = "Decrement Value", BlueprintProtected))
	float DecrementStateValue(const EPlayerStateValue Type, const float Value);

	UFUNCTION(BlueprintCallable, Category = "PlayerState", Meta = (DisplayName = "Set Value", BlueprintProtected))
	float SetStateValue(const EPlayerStateValue Type, const float Value);

public:
	/** Returns the configuration for the player state component. */
	UFUNCTION(BlueprintPure, Category = "PlayerState|Configuration", Meta = (DisplayName = "Get Configuration"))
	UPlayerStateConfiguration* GetConfiguration() const { return Configuration; }

	/** Returns the current pain of the player character. */
	UFUNCTION(BlueprintPure, Category = "PlayerState|Pain", Meta = (DisplayName = "Get Pain"))
	float GetPain() const { return Pain; }

	/** Returns the current exertion of the player character. */
	UFUNCTION(BlueprintPure, Category = "PlayerState|Exertion", Meta = (DisplayName = "Get Exertion"))
	float GetExertion() const { return Exertion; }

	/** Returns the current fear value of the player character. */
	UFUNCTION(BlueprintPure, Category = "PlayerState|Fear", Meta = (DisplayName = "Get Fear"))
	float GetFear() const { return Fear; }

	/** Returns the current vigilance value of the player character. */
	UFUNCTION(BlueprintPure, Category = "PlayerState|Vigilance", Meta = (DisplayName = "Get Vigilance"))
	float GetVigilance() const { return Vigilance; }

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "PlayerState|Events", Meta = (DisplayName = "On Update"))
	void EventOnUpdate();
};
