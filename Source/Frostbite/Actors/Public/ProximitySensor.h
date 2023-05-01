// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
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

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "Sensors", Meta = (DisplayName = "Proximity Sensor"))
class FROSTBITE_API AProximitySensor : public AActor
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogSensor, Log, All)

public:
	/** Delegate that is broadcasted when the sensor's cooldown state is changed.*/
	UPROPERTY(BlueprintAssignable, Category = "Sensor|Delegates", Meta = (DisplayName = "On Sensor State Changed"))
	FOnSensorStateChangedDelegate OnStateChanged;

protected:
	/** The scene root component for this actor. */
	UPROPERTY()
	USceneComponent* Root;
	
	/** The collision component to use for detection. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Sensor", Meta = (DisplayName = "Detection Sphere", AllowPrivateAccess = "true"))
	USphereComponent* DetectionArea;

	/** The update interval of the sensor. */
	UPROPERTY(BlueprintGetter = GetPollInterval)
	float PollInterval {0.2f};

	/** If true, the sensor's reset function needs to be called to have the sensor forfeit its triggered state. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Sensor|Detection", Meta = (DisplayName = "Requires Manual Reset"))
	bool IsManualResetRequired {true};

	/** How long the proximity sensor needs to detect an object before triggering. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Sensor|Detection", Meta = (DisplayName = "Detection Threshold",
		Units = "Seconds", ClampMin = "0", ClampMax = "10", UIMin = "0", UIMax = "10"))
	float DetectionThreshold {1.5f};
	
	float DetectionLevel {0.0f};
	
	/** The cooldown time before the sensor is able to detect actors again. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Sensor|Cooldown", Meta = (DisplayName = "Cooldown",
		Units = "Seconds"))
	float CooldownTime {3.0f};

	/** If true, the sensor is currently in cooldown and cannot detect actor. */
	UPROPERTY(BlueprintReadOnly, Category = "Sensor|Cooldown", Meta = (DisplayName = "Is Cooldown Active"))
	bool IsCooldownActive {false};
	
	/** The ignore parameters for the sensor. */
	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category = "Sensor|Ignore Parameters", Meta = (DisplayName = "Ignore Parameters"))
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

#if WITH_EDITORONLY_DATA
	/** When enables, debug visualisation is show for this actor. */
	UPROPERTY(EditInstanceOnly, Category = "Sensor|Editor", Meta = (DisplayName = "Visualize Detection Area"))
	bool EnableDebugVisualisation {false};
#endif
	
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
	UFUNCTION(BlueprintCallable, Category = "Sensor", Meta = (DisplayName = "Reset Sensor"))
	void ResetSensor();

	/** Enables the sensor. */
	UFUNCTION(BlueprintCallable, Category = "Sensor", Meta = (DisplayName = "Activate Sensor"))
	void ActivateSensor();

	/** Disables the sensor. */
	UFUNCTION(BlueprintCallable, Category = "Sensor", Meta = (DisplayName = "Deactivate Sensor"))
	void DeactivateSensor();

	/** Breaks the sensor. */
	UFUNCTION(BlueprintCallable, Category = "Sensor", Meta = (DisplayName = "Break Sensor"))
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
	UFUNCTION(BlueprintCallable, Category = "Sensor", Meta = (DisplayName = "Poll Detection Area"))
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

protected:
	/** Called after a poll. */
	UFUNCTION(BlueprintNativeEvent, Category = "Sensor", Meta = (DisplayName = "On Poll"))
	void EventOnPoll();

public:
	/** Returns if there currently is a pawn inside the sensor's range. */
	UFUNCTION(BlueprintGetter, Category = "Sensor", Meta = (DisplayName = "Is Pawn Detected"))
	FORCEINLINE bool GetIsActorDetected() const { return IsActorDetected; }

	/** Returns if the sensor is currently activated. */
	UFUNCTION(BlueprintGetter, Category = "Sensor", Meta = (DisplayName = "Is Sensor Active"))
	FORCEINLINE bool GetIsSensorActive() const { return IsSensorActive; }

	/** Returns if the sensor is currently broken. */
	UFUNCTION(BlueprintGetter, Category = "Sensor", Meta = (DisplayName = "Is Broken"))
	FORCEINLINE bool GetIsBroken() const { return IsBroken; }

	/** Returns if the sensor is currently triggered. */
	UFUNCTION(BlueprintGetter, Category = "Sensor", Meta = (DisplayName = "Is Triggered"))
	FORCEINLINE bool GetIsTriggered() const { return IsTriggered; }

	/** Returns if the sensor is currently alerted. */
	UFUNCTION(BlueprintGetter, Category = "Sensor", Meta = (DisplayName = "Is Alerted"))
	FORCEINLINE bool GetIsAlerted() const { return IsAlerted; }
	
	/** Returns the poll interval for the sensor. */
	UFUNCTION(BlueprintGetter, Category = "Sensor", Meta = (DisplayName = "Get Poll Interval"))
	FORCEINLINE float GetPollInterval() const { return PollInterval; }
};
