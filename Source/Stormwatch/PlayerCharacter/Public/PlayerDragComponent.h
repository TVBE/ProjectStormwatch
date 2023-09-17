// Copyright (c) 2022-present Barrelhouse. All rights reserved.
// Written by Nino Saglia & Tim Verberne.

#pragma once

#include "CoreMinimal.h"
#include "StormwatchMacros.h"
#include "Components/ActorComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "PlayerDragComponent.generated.h"

class UPlayerInteractionComponent;
class UCameraComponent;
class APlayerCharacter;

UCLASS(NotBlueprintable, BlueprintType, ClassGroup = "PlayerCharacter", Within = "PlayerCharacter",
	   Meta = (DisplayName = "Player Drag Component", ShortToolTip = "Component for dragging physics objects."))
	class STORMWATCH_API UPlayerDragComponent : public UPhysicsHandleComponent
{
	GENERATED_BODY()

	PLAYER_COMPONENT_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogDragComponent, Log, All)

public:
	UPROPERTY()
	UPlayerInteractionComponent* InteractionComponent;

	/** Multiplier used to change the rotation speed of the camera when dragging an object. */
	UPROPERTY()
	float CameraRotationMultiplier {1.0f};

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grab Settings")
	float LetGoDistance {300.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom Settings")
	float ZoomSpeed {0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom Settings")
	float MinZoomLevel {0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom Settings")
	float MaxZoomLevel {1000.f};

	/** The amount that the rotation speed decreases when dragging objects.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player Physics Grab")
	float CameraRotationDecreasingStrength {0.8f};

	/** Linear damping of the handle spring. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Physics Handle")
	float LinearDamping {60.0f};

	/** Linear stiffness of the handle spring */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Physics Handle")
	float LinearStiffness {40.0f};

	/** Angular damping of the handle spring */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Physics Handle")
	float AngularDamping {0.0f};

	/** Angular stiffness of the handle spring */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Physics Handle")
	float AngularStiffness {0.0f};

	/** How quickly we interpolate the physics target transform */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Physics Handle")
	float InterpolationSpeed {300.0f};

	UPROPERTY(EditAnywhere, Category = "Camera")
	UCameraComponent* Camera;

	bool ApplyForceOnCenterMass {false};

	UPROPERTY()
	class UPlayerCharacterMovementComponent* Movement;

	float Gravity;

	FVector DraggedLocationOffset;

	float TargetLocationZ;

	float DraggedComponentSize;

	/** Locations used to set the target location of the physicshandle: handle.*/
	FVector TargetLocation;

	FVector GrabOffset {0.0,0.0,0.0};

	float CurrentZoomLevel;

	float CurrentZoomAxisValue;

public:
	virtual void OnRegister() override;

	void Initialize(FPlayerDragSettings& InSettings) { Settings = InSettings; }

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Drag")
	void DragActorAtLocation(AActor* ActorToDrag, const FVector& Location);

	UFUNCTION(BlueprintCallable, Category = "Drag")
	void ReleaseActor();

private:
	void UpdateTargetLocation(float DeltaTime);

	void UpdateCameraRotationSpeed(float DeltaTime);

	void UpdateLocalConstraint();

	UFUNCTION()
	void ApplyToPhysicsHandle();

public:
	/** Returns the actor that is currently being grabbed. */
	UFUNCTION(BlueprintCallable, Category = "Player Physics Grab", Meta = (DisplayName = "Get Current Grabbed Actor"))
	AActor* GetDraggedActor() const
	{
		if (const UPrimitiveComponent * Component {GetGrabbedComponent()}) { return Component->GetOwner(); }
		return nullptr;
	}

	/** Returns the location the drag component is dragging the mesh from. */
	FVector GetDragLocation() const;
};

