// Copyright 2023 Barrelhouse

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "NightstalkerController.generated.h"

UENUM(BlueprintType)
enum class BehaviorMode : uint8
{
	RoamMode    UMETA(DisplayName = "Roam Mode", ToolTip = "In roam mode, we assume that the Nightstalker is not implicitly aware about the whereabouts of the player." ),
	HuntMode    UMETA(DisplayName = "Hunt Mode", ToolTip = "In hunt mode, we assume that the Nightstalker is implicitly aware about the whereabouts of the player."),
	AmbushMode  UMETA(DisplayName = "Ambush Mode", ToolTip = "In ambush mode, The Nightstalker is near the player and ready to strike.")
};

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = (Nightstalker))
class ANightstalkerController : public AAIController
{
	GENERATED_BODY()
	
private:
	/** The current behavior mode of the Nightstalker.*/
	UPROPERTY(BlueprintGetter = GetBehaviorMode, Category = "NightstalkerController|Behavior", Meta = (DisplayName = "Current Behavior Mode"))
	TEnumAsByte<BehaviorMode> Behavior;
	
protected:
	/** Called every frame. */
	virtual void Tick(float DeltaSeconds) override;
	
	/** Set the controller to a specific behavior mode.
	 *	@Mode The mode to switch to.
	 */
	UFUNCTION(BlueprintCallable, Category = "NightstalkerController|Behavior", Meta = (DisplayName = "Switch Behavior Mode", BlueprintProtected))
	void SwitchBehaviorMode(TEnumAsByte<BehaviorMode> Mode);

	// MODE START
	// Called everytime the Nightstalker enters roam mode.
	UFUNCTION(BlueprintNativeEvent, Category = "NightstalkerController|Behavior", Meta = (DisplayName = "Roam Mode Start"))
	void StartRoamMode();

	// Called everytime the Nightstalker enters hunt mode.
	UFUNCTION(BlueprintNativeEvent, Category = "NightstalkerController|Behavior", Meta = (DisplayName = "Hunt Mode Start"))
	void StartHuntMode();

	// Called everytime the Nightstalker enters ambush mode.
	UFUNCTION(BlueprintNativeEvent, Category = "NightstalkerController|Behavior", Meta = (DisplayName = "Ambush Mode Start"))
	void StartAmbushMode();
	
	// MODE UPDATE
	// Called every update interval while the Nightstalker is in roam mode.
	UFUNCTION(BlueprintNativeEvent, Category = "NightstalkerController|Behavior", Meta = (DisplayName = "Roam Mode Update"))
	void UpdateRoamMode();
	
	// Called every update interval while the Nightstalker is in roam mode.
	UFUNCTION(BlueprintNativeEvent, Category = "NightstalkerController|Behavior", Meta = (DisplayName = "Hunt Mode Update"))
	void UpdateHuntMode();
	
	// Called every update interval while the Nightstalker is in roam mode.
	UFUNCTION(BlueprintNativeEvent, Category = "NightstalkerController|Behavior", Meta = (DisplayName = "Ambush Mode Update"))
	void UpdateAmbushMode();

	// MODE TICK
	// Called every frame while the Nightstalker is in roam mode.
	UFUNCTION(BlueprintNativeEvent, Category = "NightstalkerController|Behavior", Meta = (DisplayName = "Roam Mode Tick"))
	void TickRoamMode();
	
	// Called every frame while the Nightstalker is in hunt mode.
	UFUNCTION(BlueprintNativeEvent, Category = "NightstalkerController|Behavior", Meta = (DisplayName = "Hunt Mode Tick"))
	void TickHuntMode();

	// Called every frame while the Nightstalker is in ambush mode.
	UFUNCTION(BlueprintNativeEvent, Category = "NightstalkerController|Behavior", Meta = (DisplayName = "Ambush Mode Tick"))
	void TickAmbushMode();

public:
	UFUNCTION(BlueprintGetter, Category = "NightstalkerController|Behavior", Meta = (DisplayName = "Current Behavior Mode"))
	FORCEINLINE TEnumAsByte<BehaviorMode> GetBehaviorMode() const {return Behavior; }
};

