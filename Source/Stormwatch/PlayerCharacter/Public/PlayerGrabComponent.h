// Copyright (c) 2022-present Barrelhouse. All rights reserved.
// Written by Nino Saglia & Tim Verberne.

#pragma once

#include "PlayerCharacterPhysicsHandleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerGrabComponent.generated.h"

class UCameraComponent;
class APlayerCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGrabbedObjectReleasedDelegate, const AActor*, GrabbedActor);

UCLASS(NotBlueprintable, BlueprintType, ClassGroup = "PlayerCharacter", Within = "PlayerCharacter",
	   Meta = (DisplayName = "Player Grab Component", ShortToolTip = "Component for grabbing physics objects."))
class UPlayerGrabComponent : public UPlayerCharacterPhysicsHandleComponent
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogGrabComponent, Log, All)

public:
	UPROPERTY(BlueprintAssignable, Category = "Delegates", Meta = (DisplayName = "On Physics Grab Component Released"))
	FOnGrabbedObjectReleasedDelegate OnGrabbedObjectReleased;

	/** If true, the grab component is currently in rotation mode. Any view input will be used to rotate the object. */
	UPROPERTY(BlueprintGetter = GetIsRotationModeActive)
	bool RotationMode;

	/** A multiplier used to change the rotation speed of the camera when grabbing an object.*/
	UPROPERTY()
	float CameraRotationMultiplier {1.0f};

private:
	/** The location where the holding hand should be relative to the physics grab component.*/
	UPROPERTY(EditInstanceOnly, Category = "Grab")
	FVector RelativeHoldingHandLocation {70.0f, 60.0f, -30.0f};

	/** The distance on which the player will let the prop go*/
	UPROPERTY(EditInstanceOnly, Category = "Grab", 
			  Meta = (Units = "Centimeters", ClampMin = "0", ClampMax = "400", UIMin = "0", UIMax = "400"))
	float LetGoDistance {150.0f};

	/** The amount that the rotation speed decreases when holding objects at a distance.*/
	UPROPERTY(EditInstanceOnly, Category = "Grab", 
			  Meta = (ClampMin = "0", ClampMax = "2", UIMin = "0", UIMax = "2"))
	float CameraRotationDecreasingStrength {0.3f};

	/** The distance where the object will move towards the hand location.*/
	UPROPERTY(EditInstanceOnly, Category = "Grab", 
			  Meta = (Units = "Centimeters", ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100"))
	float BeginHandOffsetDistance {50.0f};

	/** The minimum zoom level. */
	UPROPERTY(EditInstanceOnly, Category = "Zoom",
			  Meta = (Units = "Centimeters", ClampMin = "0", ClampMax = "400", UIMin = "0", UIMax = "400"))
	float MinZoomLevel {0.0f};

	/** The maximum zoom level. */
	UPROPERTY(EditInstanceOnly, Category = "Zoom",
			  Meta = (Units = "Centimeters", ClampMin = "0", ClampMax = "400", UIMin = "0", UIMax = "400"))
	float MaxZoomLevel {200.0f};

	/** The speed at which you can zoom. */
	UPROPERTY(EditInstanceOnly, Category = "Zoom", 
			  Meta = (ClampMin = "0"))
	float ZoomSpeed {1500.0f};

	/** The speed at which the object will return to your hand depending on how fast you walk. */
	UPROPERTY(EditInstanceOnly, Category = "Zoom", 
			  Meta = (ClampMin = "0"))
	float ReturnZoomSpeed {1000.0f};

	/** The time it takes to start priming the throw.*/
	UPROPERTY(EditInstanceOnly, Category = "Throw", 
			  Meta = (Units = "Seconds", ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	float PrimeDelay {0.1f};

	/** The curve to set the minimum and maximum strength the player can throw.*/
	UPROPERTY(EditInstanceOnly, Category = "Throw")
	UCurveFloat* ThrowStrengthCurve {nullptr};

	/** The time it will take to fully charge a throw.*/
	UPROPERTY(EditInstanceOnly, Category = "Throw", 
			  Meta = (Units = "Seconds", ClampMin = "0", ClampMax = "3", UIMin = "0", UIMax = "3"))
	float ThrowChargeTime {1.3f};

	/** The zoom level for the throw.*/
	UPROPERTY(EditInstanceOnly, Category = "Throw", 
			  Meta = (ClampMin = "0"))
	float ThrowingZoomLevel {0.0f};

	/** The speed at which the items goes to the throwing zoom level*/
	UPROPERTY(EditInstanceOnly, Category = "Throw", 
			  Meta = (ClampMin = "0"))
	float ToThrowingZoomSpeed {1000.0f};

	/**The distance the object will back up when you charge the throw.*/
	UPROPERTY(EditInstanceOnly, Category = "Throw")
	FVector ThrowingBackupVector {-10.0, 0.0, 0.0};

	/**The strength of the shaking when charging a throw.*/
	UPROPERTY(EditInstanceOnly, Category = "Throw", 
			  Meta = (ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	float ThrowingShakeSize {0.07f};

	/** Linear damping of the handle spring. */
	UPROPERTY(EditInstanceOnly, Category = "Physics Handle|Max Zoom",
			  Meta = (DisplayName = "Linear Damping", ClampMin = "0"))
	float MaxZoomLinearDamping {200.0f};

	/** Linear stiffness of the handle spring */
	UPROPERTY(EditInstanceOnly, Category = "Physics Handle|Max Zoom",
			  Meta = (DisplayName = "Linear Stiffness", ClampMin = "0"))
	float MaxZoomLinearStiffness {750.0f};

	/** Angular damping of the handle spring */
	UPROPERTY(EditInstanceOnly, Category = "Physics Handle|Max Zoom",
			  Meta = (DisplayName = "Angular Damping", ClampMin = "0"))
	float MaxZoomAngularDamping {500.0f};

	/** Angular stiffness of the handle spring */
	UPROPERTY(EditInstanceOnly, Category = "Physics Handle|Max Zoom",
			  Meta = (DisplayName = "Angular Stiffness", ClampMin = "0"))
	float MaxZoomAngularStiffness {1500.0f};

	/** How quickly we interpolate the physics target transform */
	UPROPERTY(EditInstanceOnly, Category = "Physics Handle|Max Zoom",
			  Meta = (DisplayName = "Interpolation Speed", ClampMin = "0"))
	float MaxZoomInterpolationSpeed {50.0f};

	/** Linear damping of the handle spring. */
	UPROPERTY(EditInstanceOnly, Category = "Physics Handle|Min Zoom",
			  Meta = (DisplayName = "Linear Damping", ClampMin = "0"))
	float MinZoomLinearDamping {200.0f};

	/** Linear stiffness of the handle spring */
	UPROPERTY(EditInstanceOnly, Category = "Physics Handle|Min Zoom",
			  Meta = (DisplayName = "Linear Stiffness", ClampMin = "0"))
	float MinZoomLinearStiffness {750.0f};

	/** Angular damping of the handle spring */
	UPROPERTY(EditInstanceOnly, Category = "Physics Handle|Min Zoom",
			  Meta = (DisplayName = "Angular Damping", ClampMin = "0"))
	float MinZoomAngularDamping {500.0f};

	/** Angular stiffness of the handle spring */
	UPROPERTY(EditInstanceOnly, Category = "Physics Handle|Min Zoom",
			  Meta = (DisplayName = "Angular Stiffness", ClampMin = "0"))
	float MinZoomAngularStiffness {1500.0f};

	/** How quickly we interpolate the physics target transform */
	UPROPERTY(EditInstanceOnly, Category = "Physics Handle|Min Zoom",
			  Meta = (DisplayName = "Interpolation Speed", ClampMin = "0"))
	float MinZoomInterpolationSpeed {50.0f};

	/** If true, the grab component is currently priming an object for throwing. */
	UPROPERTY(BlueprintGetter = GetIsPrimingThrow)
	bool IsPrimingThrow {false};

	UPROPERTY(BlueprintGetter = GetWillThrowOnRelease)
	bool WillThrowOnRelease;

	bool WillReleaseOnEndInteraction;

	FVector TargetLocation;

	float CurrentZoomLevel;

	float PreviousZoomLevel;

	float CurrentZoomAxisValue;

	float CurrentRotationZoomAxisValue;

	FVector LastLocation;

	FQuat CameraRelativeRotation;

	FRotator RotationDifference;

	FRotator CameraRotation;

	FQuat CameraQuat;

	FVector RotatedHandOffset;

	float WillThrowOnReleaseMultiplier;

	float PrePrimingThrowTimer;

	float ThrowingTimeLine;

	FVector ReleaseLocation;

	FQuat MouseInputRotation {0.0,0.0,0.0,1.0};

	float GrabbedComponentSize;

	FVector ThrowVelocity;

public:
	UPlayerGrabComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Grabs an actor. */
	UFUNCTION(BlueprintCallable)
	void GrabActor(AActor* ActorToGrab);

	/** Called when you release the object or place the object. */
	UFUNCTION(BlueprintCallable)
	void ReleaseObject();

	/** The third interaction which is currenty rotating the object using mouse input. */
	void BeginTetriaryInteraction();

	void EndTetriaryInteraction();

	/** Called to start charging a throw. */
	UFUNCTION(BlueprintCallable)
	void BeginPrimingThrow();

	/** Called to execute a throw during the priming of a throw. */
	UFUNCTION(BlueprintCallable)
	void PerformThrow(bool OnlyPreviewTrajectory);

	/** Gets triggered when the zoom axis value gets changed. */
	void UpdateZoomAxisValue(float ZoomAxis);

	/** Updates on tick when you are manually rotating the object. */
	void UpdateMouseImputRotation(FVector2d MouseDelta);

protected:
	virtual void BeginPlay() override;

private:
	void UpdateTargetLocationWithRotation(float DeltaTime);

	void UpdateRotatedHandOffset(FRotator& Rotation, FVector& HandOffset);

	void UpdatePhysicsHandle();

	void StopPrimingThrow();

	UFUNCTION()
	void UpdateThrowTimer(float DeltaTime);

	void VisualizeProjectilePath(AActor* ProjectileActor, FVector StartLocation, FVector LaunchVelocity);

public:
	/** Returns the actor that is currently being grabbed. */
	UFUNCTION(BlueprintCallable, Category = "Player Physics Grab", Meta = (DisplayName = "Get Current Grabbed Actor"))
	AActor* GetGrabbedActor() const
	{
		if (const UPrimitiveComponent * Component {GetGrabbedComponent()}) { return Component->GetOwner(); }
		return nullptr;
	}

	/** Returns whether the grab component is currently in rotation mode or not. */
	UFUNCTION(BlueprintGetter, Category = "GrabComponent", Meta = (DisplayName = "Is Rotation Mode Active"))
	bool GetIsRotationModeActive() const { return RotationMode; }

	/** Returns whether the grab component is priming throw on an object or not. */
	UFUNCTION(BlueprintGetter, Category = "GrabComponent", Meta = (DisplayName = "Is Priming Throw"))
	bool GetIsPrimingThrow() const { return IsPrimingThrow; }

	/** Returns whether the grab component will throw an object on release or not. */
	UFUNCTION(BlueprintGetter, Category = "GrabComponent", Meta = (DisplayName = "Is Priming Throw"))
	bool GetWillThrowOnRelease() const { return WillThrowOnRelease; }
};