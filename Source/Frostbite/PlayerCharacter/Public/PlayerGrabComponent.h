// Copyright (c) 2022-present Barrelhouse
// Written by Nino Saglia
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacterMovementComponent.h"
#include "Components/ActorComponent.h"
#include "Math/Vector.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "PlayerGrabComponent.generated.h"

class UCameraComponent;
class APlayerCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGrabbedObjectReleasedDelegate, const AActor*, GrabbedActor);

UCLASS(NotBlueprintable, BlueprintType, ClassGroup = "PlayerCharacter", Within = "PlayerCharacter",
	Meta = (DisplayName = "Player Grab Component", ShortToolTip = "Component for grabbing physics objects."))
class UPlayerGrabComponent : public UPhysicsHandleComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "GrabComponent|Delegates", Meta = (DisplayName = "On Physics Grab Component Released"))
	FOnGrabbedObjectReleasedDelegate OnGrabbedObjectReleased;
	
	/** Pointer to the configuration data asset instance for this component. */
	UPROPERTY()
	UPlayerPhysicsGrabConfiguration* Configuration;

	/** If true, the grab component is currently in rotation mode. Any view input will be used to rotate the object. */
	UPROPERTY(BlueprintGetter = GetIsRotationModeActive, Category = "GrabComponent", Meta = (DisplayName = "Is Rotation Mode Active"))
	bool RotationMode;


private:
	/** Pointer to the camera component of the player. */
	UPROPERTY(EditAnywhere, Category = "PlayerCameraReference", Meta = (DisplayName = "PlayerCamera"))
	UCameraComponent* Camera;
	

	/** If true, the grab component is currently priming an object for throwing. */
	UPROPERTY(BlueprintGetter = GetIsPrimingThrow, Category = "GrabComponent", Meta = (DisplayName = "Is Priming Throw"))
	bool IsPrimingThrow{false};

	UPROPERTY()
	bool WillThrowOnRelease;
	
	UPROPERTY()
	bool WillReleaseOnEndInteraction;
	
	UPROPERTY()
	FVector TargetLocation;
	
	UPROPERTY()
	float CurrentZoomLevel;
	
	UPROPERTY()
	float CurrentZoomAxisValue;

	UPROPERTY()
	float CurrentRotationZoomAxisValue;
	
	UPROPERTY()
	FVector LastLocation;
	
	UPROPERTY()
	FQuat CameraRelativeRotation;
	
	UPROPERTY()
	FRotator RotationDifference;
	
	UPROPERTY()
	FRotator CameraRotation;

	UPROPERTY()
	FQuat CameraQuat;
	
	UPROPERTY()
	FVector RotatedHandOffset;

	UPROPERTY()
	float WillThrowOnReleaseMultiplier;

	UPROPERTY()
	float PrePrimingThrowTimer;
	
	UPROPERTY()
	float ThrowingTimeLine;

	UPROPERTY()
	FVector ReleaseLocation;

	UPROPERTY()
	FQuat MouseInputRotation{0.0,0.0,0.0,1.0};

	UPROPERTY()
	UPlayerCharacterMovementComponent* Movement;

	UPROPERTY()
	float GrabbedComponentSize;
	
public:
	/** Grabs an actor. */
	UFUNCTION(BlueprintCallable, Category = "Player Physics Grab")
	void GrabActor(AActor* ActorToGrab);

	/** Called when you release the object or place the object */
	UFUNCTION(BlueprintCallable, Category = "Player Physics Grab")
	void ReleaseObject();

	/** Called to start charging a throw*/
	UFUNCTION(BlueprintCallable, Category = "Player Physics Grab")
	void BeginPrimingThrow();
	
	/** Called to cancel charging a throw*/
	UFUNCTION(BlueprintCallable, Category = "Player Physics Grab")
	void StopPrimingThrow();

	/** Called to execute a throw during the priming of a throw*/
	UFUNCTION(BlueprintCallable, Category = "Player Physics Grab")
	void PerformThrow();
	
	UFUNCTION()
	float CalculateThrowAngle(FVector StartLocation, FVector Target, float Velocity);

	/** Calculate the rotated hand offset based on the camera rotation.*/
	UFUNCTION(Category = "Player Physics Grab")
	void UpdateRotatedHandOffset(FRotator& Rotation, FVector& HandOffset);
	
	/** Will be updated when a component is being grabbed. */
	UFUNCTION(BlueprintCallable, Category="Player Physics Grab")
	void UpdateTargetLocationWithRotation(float DeltaTime);

	/** Gets triggered when the zoom axis value gets changed*/
	void UpdateZoomAxisValue(float ZoomAxis);

	/** Updates on tick when you are manually rotating the object.*/
	void UpdateMouseImputRotation(FVector2d MouseDelta);

	/** The third interaction which is currenty rotating the object using mouse input.*/
	void BeginTetriaryInteraction();
	void EndTetriaryInteraction();

protected:
	virtual void OnRegister() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	/** Returns the actor that is currently being grabbed. */
	UFUNCTION(BlueprintCallable, Category = "Player Physics Grab", Meta = (DisplayName = "Get Current Grabbed Actor"))
	FORCEINLINE AActor* GetGrabbedActor() const
	{
		if (const UPrimitiveComponent* Component {GetGrabbedComponent()}) { return Component->GetOwner(); }
		return nullptr; 
	}

	/** Returns whether the grab component is currently in rotation mode or not. */
	UFUNCTION(BlueprintGetter, Category = "GrabComponent", Meta = (DisplayName = "Is Rotation Mode Active"))
	FORCEINLINE bool GetIsRotationModeActive() const { return RotationMode; }

	/** Returns whether the grab component is priming throw on an object or not. */
	UFUNCTION(BlueprintGetter, Category = "GrabComponent", Meta = (DisplayName = "Is Priming Throw"))
	FORCEINLINE bool GetIsPrimingThrow() const { return IsPrimingThrow; }
};

/** Configuration asset to fine tune all variables within the physics grab component*/
UCLASS(BlueprintType, ClassGroup = "PlayerCharacter")
class FROSTBITE_API UPlayerPhysicsGrabConfiguration : public UDataAsset
{
	GENERATED_BODY()

public:
	/** The location where the holding hand should be relative to the physics grab component.*/
	UPROPERTY(EditDefaultsOnly,Category="Player Physics Grab")
	FVector RelativeHoldingHandLocation{70.0f,60.0f, -30.0f};

	/** The distance on which the player will let the prop go*/
	UPROPERTY(EditDefaultsOnly,Category="Player Physics Grab")
	float LetGoDistance{30.0f};

	/** The minimum zoom level in UE units. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Player Physics Grab")
	float MinZoomLevel{0.0f};

	/** The maximum zoom level in UE units.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Player Physics Grab")
	float MaxZoomLevel{300.0f};

	/** The distance where the object will move towards the hand location.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Player Physics Grab")
	float BeginHandOffsetDistance{50.0f};

	/**The speed at which you can zoom.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Player Physics Grab")
	float ZoomSpeed{1500.0f};

	/** The speed at which the object will return to your hand depending on how fast you walk.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Player Physics Grab")
	float WalkingRetunZoomSpeed{10.0f};

	// ... Throw variables ... 

	/** The time it takes to start priming the throw.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Player Physics Throw")
	float PrePrimingThrowDelayTime{0.5f};

	/** The minimum impulse applied when you throw an object.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Player Physics Throw")
	float MinThrowingStrength{0.0f};

	/** The maximum impulse applied when you throw an object.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Player Physics Throw")
	float MaxThrowingStrength{100.0f};
	
	/** The time it will take to fully charge a throw.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Player Physics Throw")
	float ThrowChargeTime{1.5f};
	
	/** The zoom level for the throw.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Player Physics Throw")
	float ThrowingZoomLevel{200.0f};

	/** The speed at which the items goes to the throwing zoom level*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Player Physics Throw")
	float ToThrowingZoomSpeed{0.1f};

	/**The distance the object will back up when you charge the throw.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Player Physics Throw")
	FVector ThrowingBackupVector{0.0,0.0,0.0};
	/**The strength of the shaking when charging a throw.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Player Physics Throw")
	float ThrowingShakeSize{0.1f};

	


	
	// ... PhysicsHandleSettings ... 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=PhysicsHandle)
	uint32 bSoftAngularConstraint : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PhysicsHandle)
	uint32 bSoftLinearConstraint : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PhysicsHandle)
	uint32 bInterpolateTarget : 1;

	/** Linear damping of the handle spring. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=PhysicsHandle, meta = (EditCondition = "bSoftLinearConstraint"))
	float LinearDamping{200.0f};

	/** Linear stiffness of the handle spring */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=PhysicsHandle, meta = (EditCondition = "bSoftLinearConstraint"))
	float LinearStiffness{750.0f};

	/** Angular damping of the handle spring */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=PhysicsHandle, meta = (EditCondition = "bSoftAngularConstraint"))
	float AngularDamping{500.0f};

	/** Angular stiffness of the handle spring */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=PhysicsHandle, meta = (EditCondition = "bSoftAngularConstraint"))
	float AngularStiffness{1500.0f};

	/** How quickly we interpolate the physics target transform */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=PhysicsHandle, meta = (EditCondition = "bInterpolateTarget"))
	float InterpolationSpeed{50.0f};

	
	/** Constructor with default values. */
	UPlayerPhysicsGrabConfiguration()
	{
	}
	
	void ApplyToPhysicsHandle(UPhysicsHandleComponent* PhysicsHandleComponent);
};