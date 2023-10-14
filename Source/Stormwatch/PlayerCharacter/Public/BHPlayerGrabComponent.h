// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "BHPlayerCharacterPhysicsHandleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BHPlayerGrabComponent.generated.h"

UENUM()
enum class EGrabRotationMode : uint8
{
	RotateView,
	RotateObject,
};

USTRUCT(BlueprintType)
struct FPlayerGrabSettings
{
	GENERATED_USTRUCT_BODY()

	/** The location where the holding hand should be relative to the physics grab component.*/
	UPROPERTY(EditDefaultsOnly, Category = "Grab")
	FVector RelativeHoldingHandLocation = 70.0f, 60.0f, -30.0f;

	/** The distance on which the player will let the prop go*/
	UPROPERTY(EditDefaultsOnly, Category = "Grab",
			  Meta = (Units = "Centimeters", ClampMin = "0", ClampMax = "400", UIMin = "0", UIMax = "400"))
	float LetGoDistance = 150.0f;

	/** The amount that the rotation speed decreases when holding objects at a distance.*/
	UPROPERTY(EditDefaultsOnly, Category = "Grab",
			  Meta = (ClampMin = "0", ClampMax = "2", UIMin = "0", UIMax = "2"))
	float CameraRotationDamping = 0.3f;

	/** The distance where the object will move towards the hand location.*/
	UPROPERTY(EditDefaultsOnly, Category = "Grab",
			  Meta = (Units = "Centimeters", ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100"))
	float BeginHandOffsetDistance = 50.0f;

	/** The minimum zoom level. */
	UPROPERTY(EditDefaultsOnly, Category = "Zoom",
			  Meta = (Units = "Centimeters", ClampMin = "0", ClampMax = "400", UIMin = "0", UIMax = "400"))
	float MinZoomLevel = 0.0f;

	/** The maximum zoom level. */
	UPROPERTY(EditDefaultsOnly, Category = "Zoom",
			  Meta = (Units = "Centimeters", ClampMin = "0", ClampMax = "400", UIMin = "0", UIMax = "400"))
	float MaxZoomLevel = 200.0f;

	/** The speed at which you can zoom. */
	UPROPERTY(EditDefaultsOnly, Category = "Zoom",
			  Meta = (ClampMin = "0"))
	float ZoomSpeed = 1500.0f;

	/** The speed at which the object will return to your hand depending on how fast you walk. */
	UPROPERTY(EditDefaultsOnly, Category = "Zoom",
			  Meta = (ClampMin = "0"))
	float ReturnZoomSpeed = 1000.0f;

	/** The time it takes to start priming the throw.*/
	UPROPERTY(EditDefaultsOnly, Category = "Throw",
			  Meta = (Units = "Seconds", ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	float PrimeDelay = 0.1f;

	/** The curve to set the minimum and maximum strength the player can throw.*/
	UPROPERTY(EditDefaultsOnly, Category = "Throw")
	UCurveFloat* ThrowStrengthCurve = nullptr;

	/** The time it will take to fully charge a throw.*/
	UPROPERTY(EditDefaultsOnly, Category = "Throw",
			  Meta = (Units = "Seconds", ClampMin = "0", ClampMax = "3", UIMin = "0", UIMax = "3"))
	float ThrowChargeTime = 1.3f;

	/** The zoom level for the throw.*/
	UPROPERTY(EditDefaultsOnly, Category = "Throw",
			  Meta = (ClampMin = "0"))
	float ThrowingZoomLevel = 0.0f;

	/** The speed at which the items goes to the throwing zoom level*/
	UPROPERTY(EditDefaultsOnly, Category = "Throw",
			  Meta = (ClampMin = "0"))
	float ToThrowingZoomSpeed = 1000.0f;

	/**The distance the object will back up when you charge the throw.*/
	UPROPERTY(EditDefaultsOnly, Category = "Throw")
	FVector ThrowingBackupVector = -10.0, 0.0, 0.0;

	/**The strength of the shaking when charging a throw.*/
	UPROPERTY(EditDefaultsOnly, Category = "Throw",
			  Meta = (ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	float ThrowingShakeSize = 0.07f;

	/** Linear damping of the handle spring. */
	UPROPERTY(EditDefaultsOnly, Category = "Physics Handle|Max Zoom",
			  Meta = (DisplayName = "Linear Damping", ClampMin = "0"))
	float MaxZoomLinearDamping = 200.0f;

	/** Linear stiffness of the handle spring */
	UPROPERTY(EditDefaultsOnly, Category = "Physics Handle|Max Zoom",
			  Meta = (DisplayName = "Linear Stiffness", ClampMin = "0"))
	float MaxZoomLinearStiffness = 750.0f;

	/** Angular damping of the handle spring */
	UPROPERTY(EditDefaultsOnly, Category = "Physics Handle|Max Zoom",
			  Meta = (DisplayName = "Angular Damping", ClampMin = "0"))
	float MaxZoomAngularDamping = 500.0f;

	/** Angular stiffness of the handle spring */
	UPROPERTY(EditDefaultsOnly, Category = "Physics Handle|Max Zoom",
			  Meta = (DisplayName = "Angular Stiffness", ClampMin = "0"))
	float MaxZoomAngularStiffness = 1500.0f;

	/** How quickly we interpolate the physics target transform */
	UPROPERTY(EditDefaultsOnly, Category = "Physics Handle|Max Zoom",
			  Meta = (DisplayName = "Interpolation Speed", ClampMin = "0"))
	float MaxZoomInterpolationSpeed = 50.0f;

	/** Linear damping of the handle spring. */
	UPROPERTY(EditDefaultsOnly, Category = "Physics Handle|Min Zoom",
			  Meta = (DisplayName = "Linear Damping", ClampMin = "0"))
	float MinZoomLinearDamping = 200.0f;

	/** Linear stiffness of the handle spring */
	UPROPERTY(EditDefaultsOnly, Category = "Physics Handle|Min Zoom",
			  Meta = (DisplayName = "Linear Stiffness", ClampMin = "0"))
	float MinZoomLinearStiffness = 750.0f;

	/** Angular damping of the handle spring */
	UPROPERTY(EditDefaultsOnly, Category = "Physics Handle|Min Zoom",
			  Meta = (DisplayName = "Angular Damping", ClampMin = "0"))
	float MinZoomAngularDamping = 500.0f;

	/** Angular stiffness of the handle spring */
	UPROPERTY(EditDefaultsOnly, Category = "Physics Handle|Min Zoom",
			  Meta = (DisplayName = "Angular Stiffness", ClampMin = "0"))
	float MinZoomAngularStiffness = 1500.0f;

	/** How quickly we interpolate the physics target transform */
	UPROPERTY(EditDefaultsOnly, Category = "Physics Handle|Min Zoom",
			  Meta = (DisplayName = "Interpolation Speed", ClampMin = "0"))
	float MinZoomInterpolationSpeed = 50.0f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGrabbedObjectReleasedDelegate, const AActor*, GrabbedActor);

UCLASS(NotBlueprintable, BlueprintType, ClassGroup = "PlayerCharacter", Within = "PlayerCharacter")
class UBHPlayerGrabComponent : public UBHPlayerCharacterPhysicsHandleComponent
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogGrabComponent, Log, All)

public:
	UBHPlayerGrabComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void GrabActor(AActor* ActorToGrab);
	void ReleaseActor();

	void BeginTetriaryInteraction();
	void EndTetriaryInteraction();

	void PrimeThrow();
	void PerformThrow(bool bPreviewTrajectoryOnly);

	void UpdateZoomAxisValue(float ZoomAxis);
	void UpdateMouseInputRotation(FVector2d MouseDelta);

	UFUNCTION(BlueprintCallable, Category = "Player Physics Grab", Meta = (DisplayName = "Get Current Grabbed Actor"))
	AActor* GetGrabbedActor() const
	{
		if (const UPrimitiveComponent * Component = GetGrabbedComponent()) { return Component->GetOwner(); }
		return nullptr;
	}

	UFUNCTION(BlueprintGetter)
	bool GetIsRotationModeActive() const { return bRotatingActor; }

	UFUNCTION(BlueprintGetter)
	bool GetIsPrimingThrow() const { return bPrimingThrow; }

	UFUNCTION(BlueprintGetter)
	bool GetWillThrowOnRelease() const { return bThrowOnRelease; }

	UPROPERTY(BlueprintAssignable, Category = "Delegates", Meta = (DisplayName = "On Physics Grab Component Released"))
	FOnGrabbedObjectReleasedDelegate OnGrabbedObjectReleased;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings", Meta = (ShowOnlyInnerProperties))
	FPlayerGrabSettings Settings {};

private:
	void UpdateTargetLocationWithRotation(float DeltaTime);
	void UpdateRotatedHandOffset(FRotator& Rotation, FVector& HandOffset);
	void UpdatePhysicsHandle();

	void StopPrimingThrow();

	UFUNCTION()
	void UpdateThrowTimer(float DeltaTime);

	void VisualizeProjectilePath(AActor* ProjectileActor, FVector StartLocation, FVector LaunchVelocity);

	EGrabRotationMode RotationMode = EGrabRotationMode::RotateView;

	bool bPrimingThrow = false;
	bool bThrowOnRelease;
	float PrePrimingThrowTimer;
	float ThrowingTimeLine;

	bool bReleaseOnInteractionEnd;

	FVector TargetLocation;
	FVector LastLocation;
	FVector ReleaseLocation;

	float CurrentZoomLevel;
	float PreviousZoomLevel;
	float CurrentZoomAxisValue;

	float CurrentRotationZoomAxisValue;

	FQuat CameraRelativeRotation;
	FRotator CameraRotation;
	FQuat CameraQuat;

	FRotator RotationDifference;

	FVector RotatedHandOffset;

	float WillThrowOnReleaseMultiplier;

	FQuat MouseInputRotation = 0.0,0.0,0.0,1.0;

	float GrabbedComponentSize;
	FVector ThrowVelocity;
};