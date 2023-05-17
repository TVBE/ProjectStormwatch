// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "NightstalkerController.generated.h"

class APlayerCharacter;
class ANightstalker;

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "Nightstalker")
class ANightstalkerController : public AAIController
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogNightstalkerController, Log, All)

private:
	/** Pointer to the ANightstalker instance that this controller is currently controlling. */
	UPROPERTY()
	ANightstalker* Nightstalker {nullptr};

	/** Pointer to the player character. */
	UPROPERTY(Transient)
	APlayerCharacter* PlayerCharacter {nullptr};

	/** The current distance to the player. */
	double DistanceToPlayerCharacter {0.0f};
	
	FVector LastRegisteredLocation;
	
	uint8 MaxPathHistoryLength = 50;

	/** Array of vectors where the Nightstalker has been. Can be used to backtrace it's route. */
	UPROPERTY(Transient)
	TArray<FVector> PathHistory;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void Tick(float DeltaSeconds) override;

	virtual void OnPossess(APawn* InPawn) override;

	/** Checks if a point is occluded from the perspective of another point. */
	UFUNCTION(BlueprintCallable, Category = "Occlusion", Meta = (DisplayName = "Is Occluded", Keywords = "Is Occluded Occlusion Visibility Visible", ExpandBoolAsExecs = "ReturnValue"))
	void IsOccluded(bool& ReturnValue, const FVector& PointA, const FVector& PointB, const bool DrawDebugLines = false, const float DrawDebugLineDuration = 0.0f);

	/** Performs a fast check to see if a point is occluded from the perspective of the another point.
	 *	This function is more performant than the regular 'IsActorOccluded' check, but is less accurate. */
	UFUNCTION(BlueprintCallable, Category = "Occlusion", Meta = (DisplayName = "Is Occluded (Fast)", Keywords = "Is Occluded Occlusion Visibility Visible", ExpandBoolAsExecs = "ReturnValue"))
	void IsOccludedFast(bool& ReturnValue, const  FVector& LocationA, const FVector& LocationB, const bool DrawDebugLines, const float DrawDebugLineDuration);

	UFUNCTION(BlueprintPure, Category = "Nightstalker", Meta = (CompactNodeTitle = "Nightstalker"))
	FORCEINLINE ANightstalker* GetNightstalker() { return Nightstalker; }
	
	UFUNCTION(BlueprintPure, Category = "Player Character", Meta = (CompactNodeTitle = "Player Character", Keywords = "Player Character Frostbite"))
	FORCEINLINE APlayerCharacter* GetPlayerCharacter() { return PlayerCharacter; }

	UFUNCTION(BlueprintPure, Category = "Player Character", Meta = (DisplayName = "Absolute Distance To Player"))
	FORCEINLINE double GetDistanceToPlayerCharacter() const { return DistanceToPlayerCharacter; }

	/** Returns the recent path history of the Nightstalker. */
	UFUNCTION(BlueprintPure)
	FORCEINLINE TArray<FVector> GetPathHistory() const { return PathHistory; }
};

