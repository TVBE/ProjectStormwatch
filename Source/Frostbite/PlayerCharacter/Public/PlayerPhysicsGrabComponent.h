// Copyright (c) 2022-present Barrelhouse
// Written by Nino Saglia
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacterMovementComponent.h"
#include "Components/ActorComponent.h"
#include "Math/Vector.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "PlayerPhysicsGrabComponent.generated.h"

class UCameraComponent;
class APlayerCharacter;

UCLASS(Blueprintable, ClassGroup = "PlayerCharacter", Meta = (BlueprintSpawnableComponent,
	DisplayName = "Player Grab Component", ShortToolTip = "Component for grabbing physics objects."))
class UPlayerPhysicsGrabComponent : public UPhysicsHandleComponent
{
	GENERATED_BODY()
	
public:
	/** Pointer to the configuration data asset instance for this component. */
	UPROPERTY()
	UPlayerPhysicsGrabConfiguration* Configuration;

	/** Pointer to the camera component of the player. */
	UPROPERTY(EditAnywhere, Category = "PlayerCameraReference", Meta = (DisplayName = "PlayerCamera"))
	UCameraComponent* Camera;

private:
	UPROPERTY()
	float CurrentZoomLevel;
	
	UPROPERTY()
	float CurrentZoomAxisValue;
	
	UPROPERTY()
	FVector LastLocation;
	
	UPROPERTY()
	FRotator OriginalRotation;
	
	UPROPERTY()
	FRotator RotationDifference;
	
	UPROPERTY()
	FRotator CameraRotation;
	
	UPROPERTY()
	FVector RotatedHandOffset;

	UPROPERTY()
	UPlayerCharacterMovementComponent* Movement;
	
public:
	/** The object that will be passed to the physics handle. */
	UFUNCTION(BlueprintCallable, Category = "Player Physics Grab")
	void GrabObject(AActor* ObjectToGrab);

	/** Called when you release the object or place the object */
	UFUNCTION(BlueprintCallable, Category = "Player Physics Grab")
	void ReleaseObject();

	/** Calculate the rotated hand offset based on the camera rotation.*/
	UFUNCTION(Category = "Player Physics Grab")
	void UpdateRotatedHandOffset(FRotator& Rotation, FVector& HandOffset);
	
	/** Will be updated when a component is being grabbed. */
	UFUNCTION(BlueprintCallable, Category="Player Physics Grab")
	void UpdateTargetLocationWithRotation(float DeltaTime);

	/** Gets triggered when the zoom axis value gets changed*/
	void UpdateZoomAxisValue(float ZoomAxis);

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
};

UCLASS(BlueprintType, ClassGroup = "PlayerCharacter")
class FROSTBITE_API UPlayerPhysicsGrabConfiguration : public UDataAsset
{
	GENERATED_BODY()

public:
	/** The location where the holding hand should be relative to the physics grab component.*/
	UPROPERTY(EditDefaultsOnly,Category="Player Physics Grab")
	FVector RelativeHoldingHandLocation{70.0f,40.0f, -30.0f};

	/** The minimum zoom level in UE units. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Player Physics Grab")
	float MinZoomLevel{0.0f};

	/** The maximum zoom level in UE units. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Player Physics Grab")
	float MaxZoomLevel{300.0f};

	/** The distance where the object will move towards the hand location.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Player Physics Grab")
	float BeginHandOffsetDistance{100.0f};

	/**The speed at which you can zoom.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Player Physics Grab")
	float ZoomSpeed{1500.0f};

	/** The speed at which the object will return to your hand depending on how fast you walk*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Player Physics Grab")
	float WalkingRetunZoomSpeed{10.0f};

	
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