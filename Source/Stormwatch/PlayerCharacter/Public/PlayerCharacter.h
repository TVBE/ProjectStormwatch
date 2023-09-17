// Copyright (c) 2022-present Barrelhouse. All rights reserved.
// Written by Tim Verberne.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacterController.h"
#include "PlayerCharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class APlayerCharacterController;
class UPlayerCharacterSettingsDataAsset;
class UPlayerCameraSettingsDataAsset;
class UPlayerGrabSettingsDataAsset;
class UPlayerDragSettingsDataAsset;
class UCameraComponent;
class USpotLightComponent;
class USpringArmComponent;
class UPlayerVfxComponent;
class UPlayerCameraController;
class UPlayerFlashlightComponent;
class UPlayerCharacterMovementComponent;
class UPlayerBodyCollisionComponent;
class UPlayerFootCollisionComponent;
class UNiagaraComponent;
enum class ELeftRight : uint8;
enum class EPlayerLandingType : uint8;
struct FStepData;

UENUM(BlueprintType)
enum class ECrouchToggleMode : uint8
{
	PressAndHold	UMETA(DisplayName = "Press And Hold"),
	Toggle			UMETA(DisplayName = "Toggle")
};

USTRUCT(BlueprintType)
struct FPlayerCharacterSettings
{
	GENERATED_USTRUCT_BODY()

	/** Defines the default movement speed.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "General",
			  Meta = (DisplayName = "Default Movement Speed", ForceUnits = "cm/s", 
					  ClampMin = "0", ClampMax = "400", UiMin = "0", UIMax = "400"))
	float WalkSpeed {300.f};

	/** The maximum allowed slope angle the player character can climb on. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "General", 
			  Meta = (DisplayName = "Max Walkable Floor Angle", Units = "Degrees", 
					  ClampMin = "0", ClampMax = "60", UIMin = "0", UIMax = "60"))
	float MaxWalkableFloorAngle {35.0f};

	/** The maximum step height the player can perform. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "General", 
			  Meta = (DisplayName = "Max Step Height", Units = "Centimeters", ClampMin = "0", ClampMax = "50", UIMin = "0", UIMax = "50"))
	float MaxStepHeight {30.0f};

	/** When set to enabled, the character is allowed to jump. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Jumping",
			  Meta = (DisplayName = "Enable Jumping"))
	bool JumpingEnabled {true};

	/** Defines the jump velocity of the character. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Jumping",
			  Meta = (DisplayName = "Jump Velocity", ClampMin = "0.0", UIMin = "0.0", ClampMax = "1000.0", UIMax = "1000.0", EditCondition = "JumpingEnabled"))
	float JumpVelocity {440.f};

	/** When set to enabled, the character is allowed to sprint. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sprinting",
			  Meta = (DisplayName = "Enable Sprinting"))
	bool SprintingEnabled {true};

	/** Defines the movement speed when sprinting. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sprinting",
			  Meta = (DisplayName = "Sprint Speed", ForceUnits = "cm/s", EditCondition = "SprintingEnabled", ClampMin = "400", ClampMax = "800", UiMin = "400", UIMax = "800"))
	float SprintSpeed {600.f};

	/** When set to enabled, the character is allowed to crouch. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crouching",
			  Meta = (Displayname = "Enable Crouching"))
	bool CrouchingEnabled {true};

	/** Defines the characters movement speed while crouched. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crouching",
			  Meta = (Displayname = "Crouch Speed", ForceUnits = "cm/s", EditCondition = "CrouchingEnabled", ClampMin = "0", ClampMax = "300", UiMin = "0", UIMax = "300"))
	float CrouchSpeed {200.f};

	/** Defines the crouch toggle mode. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crouching",
			  Meta = (EditCondition = "CrouchingEnabled", AdvancedDisplay = "true"))
	ECrouchToggleMode CrouchToggleMode {ECrouchToggleMode::PressAndHold};

	/** Defines the rotation rate when using a gamepad.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera",
			  Meta = (Displayname = "Gamepad Rotation Rate"))
	float RotationRate {150.f};

	/** When true, camera rotation smoothing is applied to the player. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rotation Smoothing", Meta = (DisplayName = "Enable Rotation Smoothing"))
	bool RotationSmoothingEnabled {false};

	/** The rotation smoothing speed. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rotation Smoothing",
			  Meta = (Displayname = "Rotation Smoothing Speed", ClampMin = "0", ClampMax = "20", UiMin = "0", UIMax = "20"))
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Rotation|Weight",
			  Meta = (DisplayName = "Weight Range", ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100"))
	FVector2D InteractionRotationWeightRange {FVector2D(8, 25)};

	/** The Weight scaling range. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Rotation|Weight",
			  Meta = (DisplayName = "Weight Scalars", ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	FVector2D InteractionRotationWeightScalars {FVector2D(1.0, 0.4)};

	/** The Bounding box range in which we apply interaction rotation scaling. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Rotation|Size",
			  Meta = (DisplayName = "Size Range", ClampMin = "0", ClampMax = "2000000", UIMin = "0", UIMax = "2000000"))
	FVector2D InteractionRotationSizeRange {FVector2D(500000, 1000000)};

	/** The Bounding box scaling range. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Rotation|Size",
			  Meta = (DisplayName = "Size Scalars", ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	FVector2D InteractionRotationSizeScalars {FVector2D(1.0, 0.4)};

	/** The Object distance range in which we apply interaction rotation scaling. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Rotation|Distance",
			  Meta = (DisplayName = "Distance Range", ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "200"))
	FVector2D InteractionRotationDistanceRange {FVector2D(25, 150)};

	/** The distance scaling range. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Rotation|Distance",
			  Meta = (DisplayName = "Size Scalars", ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	FVector2D InteractionRotationDistanceScalars {FVector2D(1.0, 0.5)};

	/** The offset range in which we apply interaction rotation scaling. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Rotation|Offset",
			  Meta = (DisplayName = "Offset Range", ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "200"))
	FVector2D InteractionRotationOffsetRange {FVector2D(8, 20)};

	/** The offset scaling range. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Rotation|Offset",
			  Meta = (DisplayName = "Offset Scalars", ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	FVector2D InteractionRotationOffsetScalars {FVector2D(1.0, 0.1)};

	/** The Scaling floor for weight and bounding box sized combined. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Rotation ",
			  Meta = (DisplayName = "Multiplier Floor", ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	float InteractionRotationFloor {0.4};

	/** The Weight range in which we apply interaction speed scaling. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Speed|Weight",
			  Meta = (DisplayName = "Weight Range", ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100"))
	FVector2D InteractionSpeedWeightRange {FVector2D(8, 25)};

	/** The Weight scaling range. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Speed|Weight",
			  Meta = (DisplayName = "Weight Scalars", ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	FVector2D InteractionSpeedWeightScalars {FVector2D(1.0, 0.6)};

	/** The Bounding box range in which we apply interaction speed scaling. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Speed|Size",
			  Meta = (DisplayName = "Size Range", ClampMin = "0", ClampMax = "2000000", UIMin = "0", UIMax = "2000000"))
	FVector2D InteractionSpeedSizeRange {FVector2D(500000, 1000000)};

	/** The Bounding box scaling range. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Speed|Size",
			  Meta = (DisplayName = "Size Scalars", ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	FVector2D InteractionSpeedSizeScalars {FVector2D(1.0, 0.6)};

	/** The Scaling floor for weight and bounding box sized combined. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Speed ",
			  Meta = (DisplayName = "Multiplier Floor", ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	float InteractionSpeedFloor {0.6};

	/** Constructor with default values. */
	FPlayerCharacterSettings() {}

	friend bool operator==(const FPlayerCharacterSettings& Lhs, const FPlayerCharacterSettings& Rhs)
	{
		return Lhs.WalkSpeed == Rhs.WalkSpeed
			&& Lhs.MaxWalkableFloorAngle == Rhs.MaxWalkableFloorAngle
			&& Lhs.MaxStepHeight == Rhs.MaxStepHeight
			&& Lhs.JumpingEnabled == Rhs.JumpingEnabled
			&& Lhs.JumpVelocity == Rhs.JumpVelocity
			&& Lhs.SprintingEnabled == Rhs.SprintingEnabled
			&& Lhs.SprintSpeed == Rhs.SprintSpeed
			&& Lhs.CrouchingEnabled == Rhs.CrouchingEnabled
			&& Lhs.CrouchSpeed == Rhs.CrouchSpeed
			&& Lhs.CrouchToggleMode == Rhs.CrouchToggleMode
			&& Lhs.RotationRate == Rhs.RotationRate
			&& Lhs.RotationSmoothingEnabled == Rhs.RotationSmoothingEnabled
			&& Lhs.RotationSmoothingSpeed == Rhs.RotationSmoothingSpeed
			// && Lhs.StepOverEnabled == Rhs.StepOverEnabled
			// && Lhs.StepOverWeightThreshold == Rhs.StepOverWeightThreshold
			// && Lhs.StepOverSizeThreshold == Rhs.StepOverSizeThreshold
			// && Lhs.StepOverBaseSpeedModifier == Rhs.StepOverBaseSpeedModifier
			&& Lhs.InteractionRotationWeightRange == Rhs.InteractionRotationWeightRange
			&& Lhs.InteractionRotationWeightScalars == Rhs.InteractionRotationWeightScalars
			&& Lhs.InteractionRotationSizeRange == Rhs.InteractionRotationSizeRange
			&& Lhs.InteractionRotationSizeScalars == Rhs.InteractionRotationSizeScalars
			&& Lhs.InteractionRotationDistanceRange == Rhs.InteractionRotationDistanceRange
			&& Lhs.InteractionRotationDistanceScalars == Rhs.InteractionRotationDistanceScalars
			&& Lhs.InteractionRotationOffsetRange == Rhs.InteractionRotationOffsetRange
			&& Lhs.InteractionRotationOffsetScalars == Rhs.InteractionRotationOffsetScalars
			&& Lhs.InteractionRotationFloor == Rhs.InteractionRotationFloor
			&& Lhs.InteractionSpeedWeightRange == Rhs.InteractionSpeedWeightRange
			&& Lhs.InteractionSpeedWeightScalars == Rhs.InteractionSpeedWeightScalars
			&& Lhs.InteractionSpeedSizeRange == Rhs.InteractionSpeedSizeRange
			&& Lhs.InteractionSpeedSizeScalars == Rhs.InteractionSpeedSizeScalars
			&& Lhs.InteractionSpeedFloor == Rhs.InteractionSpeedFloor;
	}

	/**
	 * Interpolates between two structs based on an alpha value.
	 * 
	 * @param Output	The result of the interpolation.
	 * @param Lhs		The left hand side of the interpolation. Corresponds to alpha = 0;
	 * @param Rhs		The right hand side of the interpolation. Corresponds to alpha = 1;
	 * @param Alpha		The alpha to use for the interpolation, is clamped between 0 and 1.
	 */
	static void Interpolate(FPlayerCharacterSettings& Output, const FPlayerCharacterSettings& Lhs, 
							const FPlayerCharacterSettings& Rhs, float Alpha)
	{
		Alpha = FMath::Clamp(Alpha, 0.0f, 1.0f);

		Output.WalkSpeed =							FMath::Lerp(Lhs.WalkSpeed, Rhs.WalkSpeed, Alpha);
		Output.MaxWalkableFloorAngle =				FMath::Lerp(Lhs.MaxWalkableFloorAngle, Rhs.MaxWalkableFloorAngle, Alpha);
		Output.MaxStepHeight =						FMath::Lerp(Lhs.MaxStepHeight, Rhs.MaxStepHeight, Alpha);
		Output.JumpVelocity =						FMath::Lerp(Lhs.JumpVelocity, Rhs.JumpVelocity, Alpha);
		Output.SprintSpeed =						FMath::Lerp(Lhs.SprintSpeed, Rhs.SprintSpeed, Alpha);
		Output.CrouchSpeed =						FMath::Lerp(Lhs.CrouchSpeed, Rhs.CrouchSpeed, Alpha);
		Output.RotationRate =						FMath::Lerp(Lhs.RotationRate, Rhs.RotationRate, Alpha);
		Output.RotationSmoothingSpeed =				FMath::Lerp(Lhs.RotationSmoothingSpeed, Rhs.RotationSmoothingSpeed, Alpha);
		Output.InteractionRotationWeightRange =		FMath::Lerp(Lhs.InteractionRotationWeightRange, Rhs.InteractionRotationWeightRange, Alpha);
		Output.InteractionRotationSizeRange =		FMath::Lerp(Lhs.InteractionRotationSizeRange, Rhs.InteractionRotationSizeRange, Alpha);
		Output.InteractionRotationDistanceRange =	FMath::Lerp(Lhs.InteractionRotationDistanceRange, Rhs.InteractionRotationDistanceRange, Alpha);
		Output.InteractionRotationOffsetRange =		FMath::Lerp(Lhs.InteractionRotationOffsetRange, Rhs.InteractionRotationOffsetRange, Alpha);
		Output.InteractionSpeedWeightRange =		FMath::Lerp(Lhs.InteractionSpeedWeightRange, Rhs.InteractionSpeedWeightRange, Alpha);
		Output.InteractionSpeedSizeRange =			FMath::Lerp(Lhs.InteractionSpeedSizeRange, Rhs.InteractionSpeedSizeRange, Alpha);
		Output.InteractionSpeedFloor =				FMath::Lerp(Lhs.InteractionSpeedFloor, Rhs.InteractionSpeedFloor, Alpha);

		Output.JumpingEnabled =						Alpha > 0.5f ? Rhs.JumpingEnabled : Lhs.JumpingEnabled;
		Output.SprintingEnabled =					Alpha > 0.5f ? Rhs.SprintingEnabled : Lhs.SprintingEnabled;
		Output.CrouchingEnabled =					Alpha > 0.5f ? Rhs.CrouchingEnabled : Lhs.CrouchingEnabled;
		Output.CrouchToggleMode =					Alpha > 0.5f ? Rhs.CrouchToggleMode : Lhs.CrouchToggleMode;
		Output.RotationSmoothingEnabled =			Alpha > 0.5f ? Rhs.RotationSmoothingEnabled : Lhs.RotationSmoothingEnabled;
	}
};

UCLASS(BlueprintType, ClassGroup = "PlayerCharacter")
class STORMWATCH_API UPlayerCharacterSettingsDataAsset : public  UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (ShowOnlyInnerProperties))
	FPlayerCharacterSettings Settings {};
};

UCLASS(Abstract, Blueprintable, BlueprintType, NotPlaceable, ClassGroup = "PlayerCharacter",
	   Meta = (DisplayName = "Player Character", ShortToolTip = "The main player character for Stormwatch."))
class STORMWATCH_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogPlayerCharacter, Log, All)

private:
	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetSettings, Category = "Settings",
			  Meta = (ShowOnlyInnerProperties))
	FPlayerCharacterSettings Settings {};

	/** The camera for the player. */
	UPROPERTY(EditAnywhere, BlueprintGetter = GetCamera, Category = "Components")
	UCameraComponent* Camera;

	/** The CameraController that handles first person camera behavior. */
	UPROPERTY(EditAnywhere, BlueprintGetter = GetCameraController, Category = "Components")
	UPlayerCameraController* CameraController;

	/** The PlayerCharacterMovementComponent that handles the PlayerCharacter's movement. */
	UPROPERTY(EditAnywhere, BlueprintGetter = GetPlayerCharacterMovement, Category = "Components",
			  Meta = (DisplayName = "Player Character Movement Component"))
	UPlayerCharacterMovementComponent* PlayerCharacterMovement;

	/** The PlayerCharacterController that is currently controlling this PlayerCharacter. */
	UPROPERTY(BlueprintGetter = GetPlayerCharacterController)
	APlayerCharacterController* PlayerCharacterController;

	/** The body collision component for the player. */
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UPlayerBodyCollisionComponent* BodyCollision;

	/** The foot collision component for the left foot. */
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UPlayerFootCollisionComponent* LeftFootCollision;

	/** The foot collision component for the right foot. */
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UPlayerFootCollisionComponent* RightFootCollision;

	/** The interaction component. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	UPlayerInteractionComponent* InteractionComponent;

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

public:
	/** Sets default values for this character's properties. */
	APlayerCharacter();

	/** Called every frame. */
	virtual void Tick(float DeltaTime) override;

	/** Called after the constructor but before BeginPlay. */
	virtual void PostInitProperties() override;

	/** Is called after all of the actor's components have been created and initialized, but before the BeginPlay function is called. */
	virtual void PostInitializeComponents() override;

	/** Performs a jump.*/
	virtual void Jump() override;

	/** Begins crouching. */
	virtual void Crouch(bool bClientSimulation) override;

	/** Stops crouching. */
	virtual void UnCrouch(bool bClientSimulation) override;

	/** Begins sprinting. */
	void StartSprinting();

	/** Stop sprinting. */
	void StopSprinting();

	/** Performs a collision query above the Pawn and returns the clearance. This will return -1.f if the query did not produce any hit results. */
	UFUNCTION(BlueprintPure)
	float GetClearanceAbovePawn() const;

	/** Checks whether the player can currently jump. */
	UFUNCTION(BlueprintPure, Meta = (DisplayName = "Can Jump"))
	bool CanPerformJump() const;

	/** Checks whether the player can currently enter crouch. */
	bool CanCrouch() const override;

	/** Checks whether the player can stand up and stop crouching. */
	UFUNCTION(BlueprintPure)
	bool CanStandUp() const;

protected:
	/** Called when the game starts or when spawned. */
	virtual void BeginPlay() override;

	/** Called after all default properties have been initialized. */
	virtual void OnConstruction(const FTransform& Transform) override;

	/** Called when the pawn is possessed by a controller. */
	virtual void PossessedBy(AController* NewController) override;

	/** Called when the pawn is ready to be destroyed or when the game ends. */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Updates the character's rotation. */
	void UpdateRotation(const float& DeltaTime);

	/** Validates the configuration data assets. */
	void ValidateConfigurationAssets();

	/** Applies the configuration data assets to the character. */
	void ApplyConfigurationAssets();

	/** Updates the character's movement speed. */
	void UpdateMovementSpeed();

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

public:
	UFUNCTION(BlueprintGetter)
	const FPlayerCharacterSettings& GetSettings() const { return Settings; }

	UFUNCTION(BlueprintGetter)
	APlayerCharacterController* GetPlayerCharacterController() const { return PlayerCharacterController; }

	UFUNCTION(BlueprintGetter)
	UCameraComponent* GetCamera() const { return Camera; }

	UFUNCTION(BlueprintGetter)
	UPlayerCameraController* GetCameraController() const { return CameraController; }

	UFUNCTION(BlueprintGetter)
	UPlayerCharacterMovementComponent* GetPlayerCharacterMovement() const { return PlayerCharacterMovement; }

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
		if (PlayerCharacterMovement)
		{
			return PlayerCharacterMovement->GetIsSprinting();
		}
		return false;
	}
};




