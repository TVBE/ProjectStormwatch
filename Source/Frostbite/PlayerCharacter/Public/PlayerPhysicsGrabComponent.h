// Copyright (c) 2022-present Barrelhouse
// Written by Nino Saglia
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
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
	//TODO: Adhere to code conventions for Frostbite: use inline comments, initialize properties with direct list assignments instead of constructor: {}.
	//TODO: Thank you! :)
	
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
	void UpdateTargetLocationWithRotation(float ZoomAxisValue);

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
	FVector RelativeHoldingHandLocation;

	/** The minimum zoom level in UE units. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Player Physics Grab")
	float MinZoomLevel;

	/** The maximum zoom level in UE units. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Player Physics Grab")
	float MaxZoomLevel;

	/** The distance where the object will move towards the hand location.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Player Physics Grab")
	float BeginHandOffsetDistance;

	/**The speed at which you can zoom.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Player Physics Grab")
	float ZoomSpeed;

	/** The speed at which the object will return to your hand depending on how fast you walk*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Player Physics Grab")
	float WalkingRetunZoomSpeed;

	
	// ... PhysicsHandleSettings ...

	/** Linear damping controls how fast the object slows down.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Movement, meta=(ClampMin = "0.0", UIMin = "0.0"))
	float LinearDamping;

	/** Angular damping controls how fast the object slows down.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Movement, meta=(ClampMin = "0.0", UIMin = "0.0"))
	float AngularDamping;

	/** The force that is used to move the grabbed object. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Movement, meta=(ClampMin = "0.0", UIMin = "0.0"))
	float InterpolationSpeed;

	/** The maximum distance that the grab can be performed from. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta=(ClampMin = "0.0", UIMin = "0.0"))
	float GrabDistance;

	/** Determines whether we should attempt to use CCD on this physics handle component. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	bool bSoftAngularConstraint;

	/** Controls how fast we move the grabbed object to the target location. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (ClampMin = "0.0", UIMin = "0.0"))
	float InterpolationSpeedThreshold;

	
	/** Constructor with default values. */
	UPlayerPhysicsGrabConfiguration()
	: RelativeHoldingHandLocation(FVector::ZeroVector)
	, MinZoomLevel(0.0f)
	, MaxZoomLevel(1000.0f)
	, ZoomSpeed(1.0f)
	, WalkingRetunZoomSpeed(200.0f)
	, LinearDamping(1.0f)
	, AngularDamping(1.0f)
	, InterpolationSpeed(1.0f)
	, GrabDistance(1000.0f)
	, bSoftAngularConstraint(false)
	, BeginHandOffsetDistance(200.0f)
	, InterpolationSpeedThreshold(1.0f)
	{
	}
	
	void ApplyToPhysicsHandle(UPhysicsHandleComponent* PhysicsHandleComponent);
};