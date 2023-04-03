// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "ProximitySensor.generated.h"

class UCapsuleComponent;
struct FTimerHandle;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnActorDetected, const AActor*, Actor, const float, Distance);

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = (Interaction), Meta = (DisplayName = "Proximity Sensor"))
class FROSTBITE_API AProximitySensor : public AActor
{ 
	GENERATED_BODY()

public:
	/** The delegate that is broadcasted when an actor is inside the sensor's range during a poll.*/
	UPROPERTY(BlueprintAssignable, Category = "Proximity Sensor", Meta = (DisplayName = "On Actor Detected"))
	FOnActorDetected OnActorDetectedDelegate;

protected:
	/** The scene root component for this actor. */
	UPROPERTY()
	USceneComponent* Root;
	
	/** The collision component to use for detection. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Proximity Sensor", Meta = (DisplayName = "Detection Cone", AllowPrivateAccess = "true"))
	UBoxComponent* DetectionBox;

	/** The update interval of the sensor. */
	UPROPERTY(EditAnywhere, BlueprintGetter = GetPollInterval, Category = "Proximity Sensor", Meta = (DisplayName = "Poll Interval"))
	float PollInterval {0.25f};
	
	/** Defines the length of the sensor's detection area.  */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Proximity Sensor|Detection Area", Meta = (DisplayName = "Length"))	
	int32 DetectionBoxLength {500};
	
	/** Defines the width of the sensor's detection area. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Proximity Sensor|Detection Area", Meta = (DisplayName = "Width"))
	int32 DetectionBoxWidth {400};
	
	/** Defines the height of the sensor's detection area. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Proximity Sensor|Detection Area", Meta = (DisplayName = "Height"))
	int32 DetectionBoxHeighth {400};

	/** Array of actor pointers that are currently overlapping the sphere. */
	UPROPERTY()
	TArray<AActor*> OverlappingActors;

	/** If true, there is currently an actor inside the detection cone of the sensor. */
	UPROPERTY(BlueprintGetter = GetIsActorDetected, Category = "Proximity Sensor", Meta = (DisplayName = "Is Actor Detected"))
	bool IsActorDetected {false};

	/** The angle for the virtual cone within the box. */
	UPROPERTY()
	float ConeAngle {0.0f};

#if WITH_EDITORONLY_DATA
	/** When enables, debug visualisation is show for this actor. */
	UPROPERTY(EditInstanceOnly, Category = "Proximity Sensor|Editor", Meta = (DisplayName = "Visualize Detection Area"))
	bool EnableDebugVisualisation {false};
#endif

private:
	/** The timer handle for the update timer. */
	UPROPERTY()
	FTimerHandle PollTimerHandle;

public:	
	AProximitySensor();

	virtual void PostInitProperties() override;
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	
protected:
	/** Performs a poll for any pawns inside the sensor's range. */
	UFUNCTION(BlueprintCallable, Category = "Proximity Sensor", Meta = (DisplayName = "Poll For Pawns"))
	virtual void Poll();
	
	/** Checks if the object within the sensor's range is occluded.*/
	UFUNCTION()
	bool IsActorOccluded(const AActor* Actor) const;

private:
	/** Calculates the cone angle based on the box extends. */
	UFUNCTION()
	float CalculateConeAngle(const UBoxComponent* BoxComponent) const;

	/** Draws debug visualisation for the sensor. */
	UFUNCTION()
	void VisualizeCone(const bool IsPersistent) const;

public:
	/** Returns if there currently is a pawn inside the sensor's range. */
	UFUNCTION(BlueprintGetter, Category = "Proximity Sensor", Meta = (DisplayName = "Is Pawn Detected"))
	FORCEINLINE bool GetIsActorDetected() const { return IsActorDetected; }
	
	/** Returns the poll interval for the sensor. */
	UFUNCTION(BlueprintGetter, Category = "Proximity Sensor", Meta = (DisplayName = "Get Poll Interval"))
	FORCEINLINE float GetPollInterval() const { return PollInterval; }

};
