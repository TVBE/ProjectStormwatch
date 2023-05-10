// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "ActorFunctionCaller.h"
#include "ProximitySensor.generated.h"

class USphereComponent;
class UCapsuleComponent;
struct FTimerHandle;

/** Defines certain ignore parameters for the sensor. */
UENUM(BlueprintType)
enum class EBProximitySensorIgnoreParameter : uint8
{
	Player						UMETA(DisplayName = "Player", ToolTip = "The player will be ignored by this sensor."),
	Nightstalker				UMETA(DisplayName = "Nightstalker", ToolTip = "The Nightstalker will be ignored by this sensor. "),
};

/** Defines the current state of the sensor. */
UENUM(BlueprintType)
enum class ESensorState : uint8
{
	Idle						UMETA(DisplayName = "Idle"),
	Alerted						UMETA(DisplayName = "Alerted"),
	Triggered					UMETA(DisplayName = "Triggered"),
	Detecting					UMETA(DisplayName = "Detecting"),
	Disabled					UMETA(DisplayName = "Disabled"),
	Broken						UMETA(DisplayName = "Broken"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSensorStateChangedDelegate, const ESensorState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSensorActivatedDelegate);

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "Sensors", Meta = (DisplayName = "Proximity Sensor"))
class FROSTBITE_API AProximitySensor : public AActor
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogSensor, Log, All)

public:
	/** Delegate that is broadcasted when the sensor's cooldown state is changed.*/
	UPROPERTY(BlueprintAssignable, Category = "Delegates", Meta = (DisplayName = "On Sensor State Changed"))
	FOnSensorStateChangedDelegate OnStateChanged;

	/** Delegate that is broadcasted when the sensor's cooldown state is changed.*/
	UPROPERTY(BlueprintAssignable, Category = "Delegates", Meta = (DisplayName = "On Sensor Activated"))
	FOnSensorActivatedDelegate OnActivation;

protected:
	/** The scene root component for this actor. */
	UPROPERTY()
	USceneComponent* Root;
	
	/** The collision component to use for detection. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sensor|Detection Area")
	UCapsuleComponent* DetectionArea;

	/** The update interval of the sensor. */
	UPROPERTY(BlueprintGetter = GetPollInterval)
	float PollInterval {0.2f};

	/** If true, the sensor's reset function needs to be called to have the sensor forfeit its triggered state. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sensor|Detection", Meta = (DisplayName = "Requires Manual Reset"))
	bool IsManualResetRequired {true};

	/** How long the proximity sensor needs to detect an object before triggering. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sensor|Detection",
		Meta = (Units = "Seconds", ClampMin = "0", ClampMax = "10", UIMin = "0", UIMax = "10"))
	float DetectionThreshold {1.5f};
	
	float DetectionLevel {0.0f};
	
	/** The cooldown time before the sensor is able to detect actors again. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sensor|Cooldown", Meta = (DisplayName = "Cooldown",
		Units = "Seconds"))
	float CooldownTime {3.0f};

	/** If true, the sensor is currently in cooldown and cannot detect actor. */
	UPROPERTY(BlueprintReadOnly, Category = "Sensor|Cooldown")
	bool IsCooldownActive {false};
	
	/** The ignore parameters for the sensor. */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Ignore Parameters")
	TArray<EBProximitySensorIgnoreParameter> IgnoreParameters;

	/** Array of actor pointers that are currently overlapping the sphere. */
	UPROPERTY()
	TArray<AActor*> OverlappingActors;

	UPROPERTY(BlueprintGetter = GetIsSensorActive)
	bool IsSensorActive {true};

	UPROPERTY(BlueprintGetter = GetIsBroken)
	bool IsBroken {false};
	
	UPROPERTY(BlueprintGetter = GetIsActorDetected)
	bool IsActorDetected {false};

	UPROPERTY(BlueprintGetter = GetIsTriggered)
	bool IsTriggered {false};

	UPROPERTY(BlueprintGetter = GetIsAlerted)
	bool IsAlerted {false};
	
	UPROPERTY()
	FTimerHandle PollTimerHandle;

	UPROPERTY()
	FTimerHandle CooldownTimerHandle;

private:
	/** The state of the sensor. */
	UPROPERTY()
	ESensorState SensorState {ESensorState::Idle};

public:	
	AProximitySensor();
	
	virtual void BeginPlay() override;
	
	/** Resets the sensor. */
	UFUNCTION(BlueprintCallable)
	void ResetSensor();

	/** Enables the sensor. */
	UFUNCTION(BlueprintCallable)
	void ActivateSensor();

	/** Disables the sensor. */
	UFUNCTION(BlueprintCallable)
	void DeactivateSensor();

	/** Breaks the sensor. */
	UFUNCTION(BlueprintCallable)
	void BreakSensor();
	
protected:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	/** Performs a poll for any pawns inside the sensor's range. */
	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Check Detection Area For Actors"))
	virtual void Poll();
	
	/** Checks if the object within the sensor's range is occluded.*/
	UFUNCTION()
	bool IsActorOccluded(const AActor* Actor) const;

	/** Starts the cooldown. */
	UFUNCTION()
	void StartCooldown();

	/** Stops the cooldown if active. */
	void StopCooldown();

	/** Sets the state of the sensor. */
	void SetState(const ESensorState NewState);

private:
	/** Called when the cooldown is finished. */
	UFUNCTION()
	void HandleCooldownFinished();

public:
	/** Returns if there currently is a pawn inside the sensor's range. */
	UFUNCTION(BlueprintGetter)
	FORCEINLINE bool GetIsActorDetected() const { return IsActorDetected; }

	/** Returns if the sensor is currently activated. */
	UFUNCTION(BlueprintGetter)
	FORCEINLINE bool GetIsSensorActive() const { return IsSensorActive; }

	/** Returns if the sensor is currently broken. */
	UFUNCTION(BlueprintGetter)
	FORCEINLINE bool GetIsBroken() const { return IsBroken; }

	/** Returns if the sensor is currently triggered. */
	UFUNCTION(BlueprintGetter)
	FORCEINLINE bool GetIsTriggered() const { return IsTriggered; }

	/** Returns if the sensor is currently alerted. */
	UFUNCTION(BlueprintGetter)
	FORCEINLINE bool GetIsAlerted() const { return IsAlerted; }
	
	/** Returns the poll interval for the sensor. */
	UFUNCTION(BlueprintGetter)
	FORCEINLINE float GetPollInterval() const { return PollInterval; }
};
