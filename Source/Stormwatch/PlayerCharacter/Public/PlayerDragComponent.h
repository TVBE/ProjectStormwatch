// Copyright (c) 2022-present Barrelhouse. All rights reserved.
// Written by Nino Saglia & Tim Verberne.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "PlayerDragComponent.generated.h"

class UPlayerInteractionComponent;
class UCameraComponent;
class APlayerCharacter;

/** Configuration asset to fine tune all variables within the drag component */
USTRUCT(BlueprintType, ClassGroup = "PlayerCharacter")
struct FPlayerDragSettings
{
	GENERATED_USTRUCT_BODY()

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

	FPlayerDragSettings() {}

	friend bool operator==(const FPlayerDragSettings& Lhs, const FPlayerDragSettings& Rhs)
	{
		return Lhs.LetGoDistance == Rhs.LetGoDistance
			&& Lhs.ZoomSpeed == Rhs.ZoomSpeed
			&& Lhs.MinZoomLevel == Rhs.MinZoomLevel
			&& Lhs.MaxZoomLevel == Rhs.MaxZoomLevel
			&& Lhs.CameraRotationDecreasingStrength == Rhs.CameraRotationDecreasingStrength
			&& Lhs.LinearDamping == Rhs.LinearDamping
			&& Lhs.LinearStiffness == Rhs.LinearStiffness
			&& Lhs.AngularDamping == Rhs.AngularDamping
			&& Lhs.AngularStiffness == Rhs.AngularStiffness
			&& Lhs.InterpolationSpeed == Rhs.InterpolationSpeed;
	}

	/**
	 * Interpolates between two structs based on an alpha value.
	 *
	 * @param Output	The result of the interpolation.
	 * @param Lhs		The left hand side of the interpolation. Corresponds to alpha = 0;
	 * @param Rhs		The right hand side of the interpolation. Corresponds to alpha = 1;
	 * @param Alpha		The alpha to use for the interpolation, is clamped between 0 and 1.
	 */
	static void Interpolate(FPlayerDragSettings& Output, const FPlayerDragSettings& Lhs,
							const FPlayerDragSettings& Rhs, float Alpha)
	{
		Output.LetGoDistance =						FMath::Lerp(Lhs.LetGoDistance, Rhs.LetGoDistance, Alpha);
		Output.ZoomSpeed =							FMath::Lerp(Lhs.ZoomSpeed, Rhs.ZoomSpeed, Alpha);
		Output.MinZoomLevel =						FMath::Lerp(Lhs.MinZoomLevel, Rhs.MinZoomLevel, Alpha);
		Output.MaxZoomLevel =						FMath::Lerp(Lhs.MaxZoomLevel, Rhs.MaxZoomLevel, Alpha);
		Output.CameraRotationDecreasingStrength =	FMath::Lerp(Lhs.CameraRotationDecreasingStrength, Rhs.CameraRotationDecreasingStrength, Alpha);
		Output.LinearDamping =						FMath::Lerp(Lhs.LinearDamping, Rhs.LinearDamping, Alpha);
		Output.LinearStiffness =					FMath::Lerp(Lhs.LinearStiffness, Rhs.LinearStiffness, Alpha);
		Output.AngularDamping =						FMath::Lerp(Lhs.AngularDamping, Rhs.AngularDamping, Alpha);
		Output.AngularStiffness =					FMath::Lerp(Lhs.AngularStiffness, Rhs.AngularStiffness, Alpha);
		Output.InterpolationSpeed =					FMath::Lerp(Lhs.InterpolationSpeed, Rhs.InterpolationSpeed, Alpha);
	}
};

UCLASS(BlueprintType, ClassGroup = "PlayerCharacter")
class STORMWATCH_API UPlayerDragSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (ShowOnlyInnerProperties))
	FPlayerDragSettings Settings {};
};

UCLASS(NotBlueprintable, BlueprintType, ClassGroup = "PlayerCharacter", Within = "PlayerCharacter",
	   Meta = (DisplayName = "Player Drag Component", ShortToolTip = "Component for dragging physics objects."))
	class STORMWATCH_API UPlayerDragComponent : public UPhysicsHandleComponent
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogDragComponent, Log, All)

public:
	UPROPERTY()
	UPlayerInteractionComponent* InteractionComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Drag Configuration")
	FPlayerDragSettings* Configuration;

	/** Multiplier used to change the rotation speed of the camera when dragging an object. */
	UPROPERTY()
	float CameraRotationMultiplier {1.0f};

private:
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
	FORCEINLINE AActor* GetDraggedActor() const
	{
		if (const UPrimitiveComponent * Component {GetGrabbedComponent()}) { return Component->GetOwner(); }
		return nullptr;
	}

	/** Returns the location the drag component is dragging the mesh from. */
	FVector GetDragLocation() const;
};

