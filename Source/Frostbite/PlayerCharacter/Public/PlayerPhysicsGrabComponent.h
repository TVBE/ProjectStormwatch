// Copyright (c) 2022-present Barrelhouse
// Written by Nino Saglia
// This source code is part of the project Frostbite
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Math/Vector.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "PlayerPhysicsGrabComponent.generated.h"

/**
 * 
 */

class UCameraComponent;
class APlayerCharacter;

UCLASS(Blueprintable, ClassGroup=PlayerCharacter, meta=(BlueprintSpawnableComponent))
class UPlayerPhysicsGrabComponent : public UPhysicsHandleComponent
{
	GENERATED_BODY()
	
public:

	// CONFIGURATION

	/** Pointer to the configuration asset for this component. */
	UPROPERTY()
	UPlayerPhysicsGrabConfiguration* Configuration;

	//The camera component of the player
	UPROPERTY(EditAnywhere, Category = "PlayerCameraReference", Meta = (DisplayName = "PlayerCamera"))
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere)
	float CurrentZoomLevel;
	
public:
	/** Returns the traveled distance of this component between every update*/
	UFUNCTION()
	double GetDeltaLocation();
	
	// The object that will be passed to the physics handle.
	UFUNCTION(BlueprintCallable, Category="Player Physics Grab")
	void GrabObject(AActor* ObjectToGrab);

	// Called when you release the object or place the object
	UFUNCTION(BlueprintCallable, Category="Player Physics Grab")
	void ReleaseObject();

	// Calculate the rotated hand offset based on the camera rotation.
	UFUNCTION(Category="Player Physics Grab")
	FVector GetRotatedHandOffset();
	
	// Will be updated when a component is being grabbed.
	UFUNCTION(BlueprintCallable, Category="Player Physics Grab")
	void UpdateTargetLocation(float ZoomAxisValue);

	void UpdateZoomAxisValue(float ZoomAxis);

protected:
	virtual void OnRegister() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//virtual void OnUnregister() override;
	//virtual void BeginPlay() override;
	//virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	// Boolean flag to track if Tick is enabled or disabled
	bool bIsTickEnabled;
	/** The current value that the mouse scroll wheel is putting out */
	float CurrentZoomAxisValue;
	/** the latest location that is being stored by the GetDeltaLocation function */
	FVector LastLocation;
	
};

UCLASS(BlueprintType, ClassGroup = (PlayerCharacter))
class FROSTBITE_API UPlayerPhysicsGrabConfiguration : public UDataAsset
{
	GENERATED_BODY()

public:

	// The location where the holding hand should be relative to the physics grab component.
	UPROPERTY(EditDefaultsOnly,Category="Player Physics Grab")
	FVector RelativeHoldingHandLocation;

	// The minimum zoom level in UE units.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Player Physics Grab")
	float MinZoomLevel;

	// The maximum zoom level in UE units.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Player Physics Grab")
	float MaxZoomLevel;

	// The distance where the object will move towards the hand location.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Player Physics Grab")
	float BeginHandOffsetDistance;

	//The speed at which you can zoom.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Player Physics Grab")
	float ZoomSpeed;

	
	// ... PhysicsHandleSettings ...

	// Linear and angular damping controls how fast the object slows down.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Movement, meta=(ClampMin = "0.0", UIMin = "0.0"))
	float LinearDamping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Movement, meta=(ClampMin = "0.0", UIMin = "0.0"))
	float AngularDamping;

	// The force that is used to move the grabbed object.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Movement, meta=(ClampMin = "0.0", UIMin = "0.0"))
	float InterpolationSpeed;

	// The maximum distance that the grab can be performed from.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta=(ClampMin = "0.0", UIMin = "0.0"))
	float GrabDistance;

	// Determines whether we should attempt to use CCD on this physics handle component.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	bool bSoftAngularConstraint;

	// Controls how fast we move the grabbed object to the target location.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (ClampMin = "0.0", UIMin = "0.0"))
	float InterpolationSpeedThreshold;

	
	/** Constructor with default values. */
	UPlayerPhysicsGrabConfiguration()
	: RelativeHoldingHandLocation(FVector::ZeroVector)
	, MinZoomLevel(0.0f)
	, MaxZoomLevel(1000.0f)
	, ZoomSpeed(1.0f)
	, LinearDamping(1.0f)
	, AngularDamping(1.0f)
	, InterpolationSpeed(1.0f)
	, GrabDistance(1000.0f)
	, bSoftAngularConstraint(false)
	, InterpolationSpeedThreshold(1.0f)
	, BeginHandOffsetDistance(200.0f)
	{
	}
	
	void ApplyToPhysicsHandle(UPhysicsHandleComponent* PhysicsHandleComponent);
};