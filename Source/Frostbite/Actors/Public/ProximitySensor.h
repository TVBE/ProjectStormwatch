// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "ProximitySensor.generated.h"

class UCapsuleComponent;
struct FTimerHandle;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPawnDetected, const APawn*, Pawn, const float, Distance);

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = (Interaction), Meta = (DisplayName = "Proximity Sensor"))
class FROSTBITE_API AProximitySensor : public AActor
{ 
	GENERATED_BODY()

public:
	/** The delegate that is broadcasted when a pawn is inside the sensor's range during a poll.*/
	UPROPERTY(BlueprintAssignable, Category = "Proximity Sensor", Meta = (DisplayName = "On Pawn Detected"))
	FOnPawnDetected OnPawnDetectedDelegate;

private:
	/** The collision component to use for detection. */
	UPROPERTY(EditAnywhere, Category = "Proximity Sensor", Meta = (DisplayName = "Detection Cone"))
	UCapsuleComponent* DetectionCone;

	/** If true, there is currently a pawn inside the detection cone of the sensor. */
	UPROPERTY(BlueprintGetter=GetIsPawnDetected, Category = "Proximity Sensor", Meta = (DisplayName = "Is Pawn Detected"))
	bool IsPawnDetected {false};

	/** The update interval of the sensor. */
	UPROPERTY(EditAnywhere, BlueprintGetter=GetPollInterval, Category = "Proximity Sensor", Meta = (DisplayName = "Poll Interval"))
	float PollInterval {0.25f};

	/** The timer handle for the update timer. */
	UPROPERTY()
	FTimerHandle PollTimerHandle;

public:	
	AProximitySensor();

	virtual void BeginPlay() override;
	
protected:
	/** Performs a poll for any pawns inside the sensor's range. */
	UFUNCTION(BlueprintCallable, Category = "Proximity Sensor", Meta = (DisplayName = "Poll For Pawns"))
	void Poll();

public:
	/** Returns if there currently is a pawn inside the sensor's range. */
	UFUNCTION(BlueprintGetter, Category = "Proximity Sensor", Meta = (DisplayName = "Is Pawn Detected"))
	FORCEINLINE bool GetIsPawnDetected() const { return IsPawnDetected; }
	
	/** Returns the poll interval for the sensor. */
	UFUNCTION(BlueprintGetter, Category = "Proximity Sensor", Meta = (DisplayName = "Get Poll Interval"))
	FORCEINLINE float GetPollInterval() const { return PollInterval; }

};
