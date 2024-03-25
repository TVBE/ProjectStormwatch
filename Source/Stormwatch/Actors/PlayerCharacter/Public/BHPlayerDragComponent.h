// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "BHPlayerCharacterComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "BHPlayerDragComponent.generated.h"

class UBHPlayerInteractionComponent;
class UCameraComponent;
class ABHPlayerCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnObjectDraggedSignature, const UObject*, DraggedObject, const UPrimitiveComponent*, DraggedComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDraggedObjectReleasedSignature, const UObject*, ReleasedObject, const UPrimitiveComponent*, ReleasedComponent);

UCLASS(NotBlueprintable, BlueprintType, ClassGroup = "BHPlayerCharacter")
	class STORMWATCH_API UBHPlayerDragComponent : public UPhysicsHandleComponent, public FBHPlayerCharacterComponent
{
	GENERATED_BODY()

public:
	UBHPlayerDragComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "BHPlayerDragComponent")
	void Drag(UObject* Object);

	UFUNCTION(BlueprintCallable, Category = "BHPlayerGrabComponent")
	void Release();
	
	UFUNCTION(BlueprintPure, Category = "BHPlayerDragComponent")
	bool IsHoldingObject() const;
	
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnObjectDraggedSignature OnObjectDragged;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnDraggedObjectReleasedSignature OnObjectReleased;

	// FVector GetDragLocation() const;

	UPROPERTY()
	float CameraRotationMultiplier = 1.0f;

private:
	// Restrict dragging to only the Actor's root component. 
	// This does not have to be the implementer of IBHDraggableObject.
	UPROPERTY(EditDefaultsOnly, Category = "Dragging")
	bool bAllowRootOnly = true;
	
	// void UpdateTargetLocation(float DeltaTime);
	//
	// void UpdateCameraRotationSpeed(float DeltaTime);
	//
	// void UpdateLocalConstraint();

	// UFUNCTION()
	// void UpdatePhysicsHandle();

	/** The distance at which point a dragged object will be automatically released. */
	UPROPERTY(EditInstanceOnly, Category = "Grab Settings", 
			  Meta = (DisplayName = "Release Distance", Units = "Centimeters", ClampMin = "0", ClampMax = "500", UIMin = "0", UIMax = "500"))
	float AutoReleaseDistance = 300.f;

	UPROPERTY(EditInstanceOnly, Category = "Zoom Settings", 
			  Meta = (ClampMin = "0"))
	float ZoomSpeed = 0.0f;

	UPROPERTY(EditInstanceOnly, Category = "Zoom Settings", 
			  Meta = (ClampMin = "0"))
	float MinZoomLevel = 0.0f;

	UPROPERTY(EditInstanceOnly, Category = "Zoom Settings", 
			  Meta = (ClampMin = "0"))
	float MaxZoomLevel = 1000.f;

	/** The amount that the rotation speed decreases when dragging objects.*/
	UPROPERTY(EditInstanceOnly, Category = "Player Physics Grab", 
			  Meta = (ClampMin = "0"))
	float CameraRotationDamping = 0.8f;

	bool bApplyForceOnCenterMass = false;

	FVector DraggedLocationOffset;

	float TargetLocationZ;

	float DraggedComponentSize;

	FVector TargetLocation;

	FVector GrabOffset {0.0,0.0,0.0};

	float CurrentZoomLevel;

	float CurrentZoomAxisValue;
};

