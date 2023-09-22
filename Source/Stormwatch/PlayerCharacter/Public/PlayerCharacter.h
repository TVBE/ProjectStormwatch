// Copyright (c) 2022-present Barrelhouse. All rights reserved.
// Written by Tim Verberne.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacterController.h"
#include "PlayerMovementComponent.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

UENUM(BlueprintType)
enum class ECrouchToggleMode : uint8
{
	PressAndHold	UMETA(DisplayName = "Press And Hold"),
	Toggle			UMETA(DisplayName = "Toggle")
};

UCLASS(Abstract, Blueprintable, BlueprintType, NotPlaceable, ClassGroup = "PlayerCharacter",
	   Meta = (DisplayName = "Player Character", ShortToolTip = "The main player character for Stormwatch."))
class STORMWATCH_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

	virtual void Tick(float DeltaTime) override;

	virtual void Jump() override;

	virtual void Crouch(bool bClientSimulation) override;
	virtual void UnCrouch(bool bClientSimulation) override;

	void StartSprinting();
	void StopSprinting();

	/** Performs a collision query above the Pawn and returns the clearance. This will return -1.f if the query did not produce any hit results. */
	UFUNCTION(BlueprintPure)
	float GetClearanceAbovePawn() const;

	UFUNCTION(BlueprintPure, Meta = (DisplayName = "Can Jump"))
	bool CanPerformJump() const;

	bool CanCrouch() const override;

	UFUNCTION(BlueprintPure)
	bool CanStandUp() const;

	UFUNCTION(BlueprintGetter)
	float GetTargetSpeed() const { return TargetSpeed; }

	UFUNCTION(BlueprintGetter)
	float GetScaledSpeed() const { return ScaledSpeed; }

	UFUNCTION(BlueprintGetter)
	bool GetIsTurningInPlace() const { return IsTurningInPlace; }

	UFUNCTION(BlueprintGetter)
	float GetYawDelta() const { return YawDelta; }

	UFUNCTION(BlueprintPure)
	bool IsSprinting() const
	{
		if (PlayerMovement)
		{
			return PlayerMovement->GetIsSprinting();
		}
		return false;
	}

	float GetWalkSpeed() const { return WalkSpeed; }

	float GetMaxWalkableFloorAngle() const { return MaxWalkableFloorAngle; }

	float GetMaxStepHeight() const { return MaxStepHeight; }

	bool IsJumpingEnabled() const { return JumpingEnabled; }
	float GetJumpVelocity() const { return JumpVelocity; }

	bool IsSprintingEnabled() const { return SprintingEnabled; }
	float GetSprintSpeed() const { return SprintSpeed; }

	bool IsCrouchingEnabled() const { return CrouchingEnabled; }
	float GetCrouchSpeed() const { return CrouchSpeed; }

	float GetRotationRate() const { return RotationRate; }

	/** We assume that the following pointers are always valid during the lifetime of a PlayerCharacter instance.
	 *  This allows us to skip ptr validity checks in any object that calls these getter functions.
	 *  Should any of these be null, we explicitly crash the program. */
	UFUNCTION(BlueprintGetter)
	class UCameraComponent* GetCamera() const
	{
		check(Camera);
		return Camera;
	}

	UFUNCTION(BlueprintGetter)
	class UPlayerCameraController* GetCameraController() const
	{
		check(CameraController);
		return CameraController;
	}

	UFUNCTION(BlueprintGetter)
	class UPlayerMovementComponent* GetPlayerMovement() const
	{
		check(PlayerMovement);
		return PlayerMovement;
	}

	UFUNCTION(BlueprintGetter)
	UPlayerInteractionComponent* GetInteractionComponent() const
	{
		check(InteractionComponent);
		return InteractionComponent;
	}

	UFUNCTION(BlueprintGetter)
	class UPlayerUseComponent* GetUseComponent() const
	{
		check(UseComponent);
		return UseComponent;
	}

	UFUNCTION(BlueprintGetter)
	class UPlayerGrabComponent* GetGrabComponent() const
	{
		check(GrabComponent);
		return GrabComponent;
	}

	UFUNCTION(BlueprintGetter)
	class UPlayerDragComponent* GetDragComponent() const
	{
		check(DragComponent);
		return DragComponent;
	}

	UFUNCTION(BlueprintGetter)
	class UPlayerInventoryComponent* GetInventoryComponent() const
	{
		check(InventoryComponent);
		return InventoryComponent;
	}

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void PossessedBy(AController* NewController) override;

	void UpdateRotation(const float& DeltaTime);
	void UpdateMovementSpeed();

	/** Defines the default movement speed.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "General",
			  Meta = (DisplayName = "Default Movement Speed", ForceUnits = "cm/s",
					  ClampMin = "0", ClampMax = "400", UiMin = "0", UIMax = "400"))
	float WalkSpeed {300.f};

	/** The maximum allowed slope angle the player character can climb on. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "General",
			  Meta = (Units = "Degrees", ClampMin = "0", ClampMax = "60", UIMin = "0", UIMax = "60"))
	float MaxWalkableFloorAngle {35.0f};

	/** The maximum step height the player can perform. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "General",
			  Meta = (Units = "Centimeters", ClampMin = "0", ClampMax = "50", UIMin = "0", UIMax = "50"))
	float MaxStepHeight {30.0f};

	/** When set to enabled, the character is allowed to jump. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jumping",
			  Meta = (DisplayName = "Enable Jumping"))
	bool JumpingEnabled {true};

	/** Defines the jump velocity of the character. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jumping",
			  Meta = (ClampMin = "0.0", UIMin = "0.0", ClampMax = "1000.0", UIMax = "1000.0", EditCondition = "JumpingEnabled"))
	float JumpVelocity {440.f};

	/** When set to enabled, the character is allowed to sprint. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sprinting",
			  Meta = (DisplayName = "Enable Sprinting"))
	bool SprintingEnabled {true};

	/** Defines the movement speed when sprinting. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sprinting",
			  Meta = (ForceUnits = "cm/s", EditCondition = "SprintingEnabled", ClampMin = "400", ClampMax = "800", UiMin = "400", UIMax = "800"))
	float SprintSpeed {600.f};

	/** When set to enabled, the character is allowed to crouch. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crouching",
			  Meta = (Displayname = "Enable Crouching"))
	bool CrouchingEnabled {true};

	/** Defines the characters movement speed while crouched. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crouching",
			  Meta = (ForceUnits = "cm/s", EditCondition = "CrouchingEnabled", ClampMin = "0", ClampMax = "300", UiMin = "0", UIMax = "300"))
	float CrouchSpeed {200.f};

	/** Defines the crouch toggle mode. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crouching",
			  Meta = (EditCondition = "CrouchingEnabled", AdvancedDisplay = "true"))
	ECrouchToggleMode CrouchToggleMode {ECrouchToggleMode::PressAndHold};

	/** Defines the rotation rate when using a gamepad.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera",
			  Meta = (Displayname = "Gamepad Rotation Rate"))
	float RotationRate {150.f};

	/** When true, camera rotation smoothing is applied to the player. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rotation Smoothing", Meta = (DisplayName = "Enable Rotation Smoothing"))
	bool RotationSmoothingEnabled {false};

	/** The rotation smoothing speed. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rotation Smoothing",
			  Meta = (ClampMin = "0", ClampMax = "20", UiMin = "0", UIMax = "20"))
	float RotationSmoothingSpeed {8.0f};

	/** If true, the player is slowed down when stepping over larger physics bodies. */
	//UPROPERTY(BlueprintReadOnly, Category = "Collision|StepOver", 
	//		  Meta = (DisplayName = "Slow Down Character When Stepping Over Physics Bodies"))
	//bool StepOverEnabled {true};

	/** The minimum weight of an object required to slow down the player. */
	//UPROPERTY(BlueprintReadOnly, Category = "Collision|StepOver",
	//		  Meta = (DisplayName = "Weight Threshold", Units = "Kilograms", ClampMin = "0", EditCondition = "StepOverEnabled"))
	//float StepOverWeightThreshold {10.0f};

	/** The minimum size of an object required to slow down the player */
	//UPROPERTY(BlueprintReadOnly, Category = "Collision|StepOver",
	//		  Meta = (DisplayName = "Size Threshold", ClampMin = "0", EditCondition = "StepOverEnabled"))
	//float StepOverSizeThreshold {100.0f};

	/** The base speed modifier to apply when the player is performing step-overs. */
	//UPROPERTY(BlueprintReadOnly, Category = "Collision|StepOver",
	//		  Meta = (DisplayName = "Base Modifier", ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1", EditCondition = "StepOverEnabled"))
	//float StepOverBaseSpeedModifier {0.6};

	/** The Weight range in which we apply interaction rotation scaling.
	 *  X defines the minimum amount of weight before we start fading in the scaling.
	 *  Y defines the maximum amount of weight for when we start applying maximum scaling. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Rotation|Weight",
			  Meta = (DisplayName = "Weight Range", ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100"))
	FVector2D InteractionRotationWeightRange {FVector2D(8, 25)};

	/** The Weight scaling range. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Rotation|Weight",
			  Meta = (DisplayName = "Weight Scalars", ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	FVector2D InteractionRotationWeightScalars {FVector2D(1.0, 0.4)};

	/** The Bounding box range in which we apply interaction rotation scaling. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Rotation|Size",
			  Meta = (DisplayName = "Size Range", ClampMin = "0", ClampMax = "2000000", UIMin = "0", UIMax = "2000000"))
	FVector2D InteractionRotationSizeRange {FVector2D(500000, 1000000)};

	/** The Bounding box scaling range. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Rotation|Size",
			  Meta = (DisplayName = "Size Scalars", ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	FVector2D InteractionRotationSizeScalars {FVector2D(1.0, 0.4)};

	/** The Object distance range in which we apply interaction rotation scaling. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Rotation|Distance",
			  Meta = (DisplayName = "Distance Range", ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "200"))
	FVector2D InteractionRotationDistanceRange {FVector2D(25, 150)};

	/** The distance scaling range. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Rotation|Distance",
			  Meta = (DisplayName = "Size Scalars", ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	FVector2D InteractionRotationDistanceScalars {FVector2D(1.0, 0.5)};

	/** The offset range in which we apply interaction rotation scaling. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Rotation|Offset",
			  Meta = (DisplayName = "Offset Range", ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "200"))
	FVector2D InteractionRotationOffsetRange {FVector2D(8, 20)};

	/** The offset scaling range. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Rotation|Offset",
			  Meta = (DisplayName = "Offset Scalars", ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	FVector2D InteractionRotationOffsetScalars {FVector2D(1.0, 0.1)};

	/** The Scaling floor for weight and bounding box sized combined. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Rotation ",
			  Meta = (DisplayName = "Multiplier Floor", ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	float InteractionRotationFloor {0.4};

	/** The Weight range in which we apply interaction speed scaling. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Speed|Weight",
			  Meta = (DisplayName = "Weight Range", ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100"))
	FVector2D InteractionSpeedWeightRange {FVector2D(8, 25)};

	/** The Weight scaling range. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Speed|Weight",
			  Meta = (DisplayName = "Weight Scalars", ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	FVector2D InteractionSpeedWeightScalars {FVector2D(1.0, 0.6)};

	/** The Bounding box range in which we apply interaction speed scaling. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Speed|Size",
			  Meta = (DisplayName = "Size Range", ClampMin = "0", ClampMax = "2000000", UIMin = "0", UIMax = "2000000"))
	FVector2D InteractionSpeedSizeRange {FVector2D(500000, 1000000)};

	/** The Bounding box scaling range. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Speed|Size",
			  Meta = (DisplayName = "Size Scalars", ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	FVector2D InteractionSpeedSizeScalars {FVector2D(1.0, 0.6)};

	/** The Scaling floor for weight and bounding box sized combined. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Speed ",
			  Meta = (DisplayName = "Multiplier Floor", ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	float InteractionSpeedFloor {0.6};

private:
	/** The camera for the player. */
	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetCamera, Category = "Components")
	UCameraComponent* Camera;

	/** The CameraController that handles first person camera behavior. */
	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetCameraController, Category = "Components")
	UPlayerCameraController* CameraController;

	/** The PlayerMovementComponent that handles the PlayerCharacter's movement. */
	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetPlayerMovement, Category = "Components",
			  Meta = (DisplayName = "Player Movement Component"))
	UPlayerMovementComponent* PlayerMovement;

	/** The interaction component. */
	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetInteractionComponent, Category = "Components")
	UPlayerInteractionComponent* InteractionComponent;

	/** The use component that is used to use actors. */
	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetUseComponent, Category = "Components")
	UPlayerUseComponent* UseComponent;

	/** The physics grab component that is used to grab actors. */
	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetGrabComponent, Category = "Components")
	UPlayerGrabComponent* GrabComponent;

	/** The physics drag component that is used to drag actors. */
	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetDragComponent, Category = "Components")
	UPlayerDragComponent* DragComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetInventoryComponent, Category = "Components")
	UPlayerInventoryComponent* InventoryComponent;

	/** The body collision component for the player. */
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class UPlayerBodyCollisionComponent* BodyCollision;

	/** The foot collision component for the left foot. */
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class UPlayerFootCollisionComponent* LeftFootCollision;

	/** The foot collision component for the right foot. */
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UPlayerFootCollisionComponent* RightFootCollision;

	/** The target speed of the character. */
	UPROPERTY(BlueprintGetter = GetTargetSpeed)
	float TargetSpeed {0.0f};

	/** The scaled speed of the character. */
	UPROPERTY(BlueprintGetter = GetScaledSpeed)
	float ScaledSpeed {0.0f};

	/** If true, the character is currently turning in place. */
	UPROPERTY(BlueprintGetter = GetIsTurningInPlace)
	bool IsTurningInPlace {false};

	/** The yaw delta value used for turn-in-place rotation. */
	UPROPERTY(BlueprintGetter = GetYawDelta)
	float YawDelta {0.f};

	/** The timer handle for the hard and heavy landing stun duration. */
	UPROPERTY()
	FTimerHandle FallStunTimer;

private:
	/** Updates the character's yaw delta. */
	UFUNCTION()
	void UpdateYawDelta();

	/** 
	 *  Returns a scaled yaw delta value that is used for turn-in-place rotation.
	 * 
	 *	@param YawDelta The delta yaw rotation between the player's control rotation and the character's mesh rotation.
	 *	@param DeltaTime The frame time in seconds.
	 *	@param Factor A multiplier that affects the rotation speed.
	 *	@param Clamp The maximum allowed YawDelta angle before the rotation speed should be clamped to prevent the camera from rotation too much in relation to the character's neck. //TODO: This doesn't work properly currently.
	 *	@return A float value representing a rotator's yaw axis.
	 */
	static float CalculateTurnInPlaceRotation(const float YawDelta, const float DeltaTime, const float Factor, const float Clamp);

	/** Handles the landing callback from the player character movement component. */
	UFUNCTION()
	void HandleLanding(EPlayerLandingType Value);

	/** Handles the ending of a landing. */
	UFUNCTION()
	void HandleLandingEnd();
};




