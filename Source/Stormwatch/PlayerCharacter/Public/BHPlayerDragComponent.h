// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "BHPlayerCharacterPhysicsHandleComponent.h"
#include "BHPlayerDragComponent.generated.h"

class UBHPlayerInteractionComponent;
class UCameraComponent;
class ABHPlayerCharacter;

UCLASS(NotBlueprintable, BlueprintType, ClassGroup = "PlayerCharacter", Within = "PlayerCharacter")
	class STORMWATCH_API UBHPlayerDragComponent : public UBHPlayerCharacterPhysicsHandleComponent
{
	GENERATED_BODY()

public:
	UBHPlayerDragComponent() {};

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Drag")
	void DragActorAtLocation(AActor* ActorToDrag, const FVector& Location);

	UFUNCTION(BlueprintCallable, Category = "Drag")
	void ReleaseActor();

	UFUNCTION(BlueprintCallable, Category = "Player Physics Grab", Meta = (DisplayName = "Get Current Grabbed Actor"))
	AActor* GetDraggedActor() const
	{
		if (const UPrimitiveComponent * Component = GetGrabbedComponent()) { return Component->GetOwner(); }
		return nullptr;
	}

	FVector GetDragLocation() const;

	UPROPERTY()
	float CameraRotationMultiplier = 1.0f;

protected:
	virtual void OnRegister() override;

	virtual void BeginPlay() override;

private:
	void UpdateTargetLocation(float DeltaTime);

	void UpdateCameraRotationSpeed(float DeltaTime);

	void UpdateLocalConstraint();

	UFUNCTION()
	void UpdatePhysicsHandle();

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

	/** Locations used to set the target location of the physicshandle: handle.*/
	FVector TargetLocation;

	FVector GrabOffset = 0.0,0.0,0.0;

	float CurrentZoomLevel;

	float CurrentZoomAxisValue;
};

