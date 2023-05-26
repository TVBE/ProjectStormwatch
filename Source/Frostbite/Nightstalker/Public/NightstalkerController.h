// Copyright (c) 2022-present Barrelhouse. All rights reserved.
// Written by Tim Verberne.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "NightstalkerController.generated.h"

struct FTimerHandle;

UENUM(BlueprintType)
enum class EBehaviorMode : uint8
{
	RoamMode    UMETA(DisplayName = "Roam Mode", ToolTip = "In roam mode, we assume that the Nightstalker is not implicitly aware about the whereabouts of the player." ),
	StalkMode    UMETA(DisplayName = "stalk Mode", ToolTip = "In stalk mode, we assume that the Nightstalker is implicitly aware about the whereabouts of the player."),
	AmbushMode  UMETA(DisplayName = "Ambush Mode", ToolTip = "In ambush mode, The Nightstalker is near the player and ready to strike.")
};

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = (Nightstalker))
class ANightstalkerController : public AAIController
{
	GENERATED_BODY()
	
private:
	/** The current behavior mode of the Nightstalker.*/
	UPROPERTY(BlueprintGetter = GetBehaviorMode, Category = "NightstalkerController|Behavior", Meta = (DisplayName = "Current Behavior Mode"))
	EBehaviorMode BehaviorMode;

	/** The update interval for roam mode. */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "NightstalkerController|Behavior", Meta = (DisplayName = "Roam Mode Update Interval", AllowPrivateAccess = "true"))
	float RoamModeUpdateInterval {1.65f};
	
	/** The update interval for stalk mode. */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "NightstalkerController|Behavior", Meta = (DisplayName = "Stalk Mode Update Interval", AllowPrivateAccess = "true"))
	float StalkModeUpdateInterval {1.25f};
	
	/** The update interval for ambush mode.*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "NightstalkerController|Behavior", Meta = (DisplayName = "Ambush Mode Update Interval", AllowPrivateAccess = "true"))
	float AmbushModeUpdateInterval {1.1f};
	
	/** The timer handle for the behavior mode update. */
	UPROPERTY(BlueprintReadOnly, Category = "NightstalkerController|Timers", Meta = (DisplayName = "Behavior Update Timer Handle", AllowPrivateAccess = "true"))
	FTimerHandle BehaviorUpdateTimerHandle;
	
protected:
	/** Called every frame. */
	virtual void Tick(float DeltaSeconds) override;

	/** Called when the game ends for the controller. */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	/** Set the controller to a specific behavior mode.
	 *	@Mode The mode to switch to.
	 */
	UFUNCTION(BlueprintCallable, Category = "NightstalkerController|Behavior", Meta = (DisplayName = "Switch Behavior Mode", BlueprintProtected))
	void SwitchBehaviorMode(const EBehaviorMode Mode);

	// MODE START
	/** Called everytime the Nightstalker enters roam mode. */
	UFUNCTION(BlueprintNativeEvent, Category = "NightstalkerController|Behavior", Meta = (DisplayName = "Roam Mode Start"))
	void StartRoamMode();

	/** Called everytime the Nightstalker enters stalk mode. */
	UFUNCTION(BlueprintNativeEvent, Category = "NightstalkerController|Behavior", Meta = (DisplayName = "Stalk Mode Start"))
	void StartStalkMode();

	/** Called everytime the Nightstalker enters ambush mode. */
	UFUNCTION(BlueprintNativeEvent, Category = "NightstalkerController|Behavior", Meta = (DisplayName = "Ambush Mode Start"))
	void StartAmbushMode();
	
	// MODE UPDATE
	/** Called every update interval while the Nightstalker is in roam mode. */
	UFUNCTION(BlueprintNativeEvent, Category = "NightstalkerController|Behavior", Meta = (DisplayName = "Roam Mode Update"))
	void UpdateRoamMode();
	
	/** Called every update interval while the Nightstalker is in roam mode. */
	UFUNCTION(BlueprintNativeEvent, Category = "NightstalkerController|Behavior", Meta = (DisplayName = "Stalk Mode Update"))
	void UpdateStalkMode();
	
	/** Called every update interval while the Nightstalker is in roam mode. */
	UFUNCTION(BlueprintNativeEvent, Category = "NightstalkerController|Behavior", Meta = (DisplayName = "Ambush Mode Update"))
	void UpdateAmbushMode();

	// MODE TICK
	/** Called every frame while the Nightstalker is in roam mode. */
	UFUNCTION(BlueprintNativeEvent, Category = "NightstalkerController|Behavior", Meta = (DisplayName = "Roam Mode Tick"))
	void TickRoamMode();
	
	/** Called every frame while the Nightstalker is in stalk mode. */
	UFUNCTION(BlueprintNativeEvent, Category = "NightstalkerController|Behavior", Meta = (DisplayName = "Stalk Mode Tick"))
	void TickStalkMode();

	/** Called every frame while the Nightstalker is in ambush mode. */
	UFUNCTION(BlueprintNativeEvent, Category = "NightstalkerController|Behavior", Meta = (DisplayName = "Ambush Mode Tick"))
	void TickAmbushMode();

private:
	/** Called every update interval. */
	UFUNCTION()
	void OnBehaviorModeUpdate();

public:
	UFUNCTION(BlueprintGetter, Category = "NightstalkerController|Behavior", Meta = (DisplayName = "Current Behavior Mode"))
	FORCEINLINE EBehaviorMode GetBehaviorMode() const {return BehaviorMode; }
};

