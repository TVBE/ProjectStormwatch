// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BHNightstalker.h"
#include "BHNightstalkerController.generated.h"

class ABHPlayerCharacter;
class ABHNightstalker;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerPerceptionChangedDelegate, bool, IsPlayerDetected);

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "Nightstalker")
class STORMWATCH_API ABHNightstalkerController : public AAIController
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogNightstalkerController, Log, All)

public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Delegates")
	FOnPlayerPerceptionChangedDelegate OnPlayerPerceptionChanged;

private:
	/** Pointer to the ANightstalker instance that this controller is currently controlling. */
	UPROPERTY()
	ABHNightstalker* Nightstalker = nullptr;

	/** Pointer to the player character. */
	UPROPERTY(Transient)
	ABHPlayerCharacter* PlayerCharacter = nullptr;

	/** The tick rate for the behavior tick. */
	UPROPERTY(EditAnywhere, Category = "Behavior")
	float BehaviorTickRate = 5.0f;

	float BehaviorTickInterval;
	
	FTimerHandle BehaviorTickTimerHandle;

	/** The current distance to the player. */
	double DistanceToPlayerCharacter = 0.0f;
	
	FVector LastRegisteredLocation;
	
	uint8 MaxPathHistoryLength = 50;

	/** Array of vectors where the Nightstalker has been. Can be used to backtrace it's route. */
	UPROPERTY(Transient)
	TArray<FVector> PathHistory;

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintNativeEvent)
	void BehaviorTick(float DeltaSeconds);

	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION(BlueprintCallable, Category = "Logging", Meta = (DevelopmentOnly))
	void LogText(const FString& Text);
	
	UFUNCTION(BlueprintPure, Category = "Nightstalker", Meta = (CompactNodeTitle = "Nightstalker"))
	FORCEINLINE ABHNightstalker* GetNightstalker() { return Nightstalker; }
	
	UFUNCTION(BlueprintPure, Category = "Player Character", Meta = (CompactNodeTitle = "Player Character", Keywords = "Player Character Stormwatch"))
	FORCEINLINE ABHPlayerCharacter* GetPlayerCharacter() { return PlayerCharacter; }

	UFUNCTION(BlueprintPure, Category = "Player Character", Meta = (DisplayName = "Absolute Distance To Player"))
	FORCEINLINE double GetDistanceToPlayerCharacter() const { return DistanceToPlayerCharacter; }
	
	/** Returns the recent path history of the Nightstalker. */
	UFUNCTION(BlueprintPure)
	FORCEINLINE TArray<FVector> GetPathHistory() const { return PathHistory; }
};

