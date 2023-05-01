// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacterController.h"
#include "PlayerCharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class UPlayerCharacterConfiguration;
class UPlayerStateConfiguration;
class APlayerCharacterController;
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

UCLASS(Abstract, Blueprintable, BlueprintType, NotPlaceable, ClassGroup = "PlayerCharacter", Meta =
	(DisplayName = "Player Character", ShortToolTip = "The main player character for Frostbite."))
class FROSTBITE_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()
	
	DECLARE_LOG_CATEGORY_CLASS(LogPlayerCharacter, Log, All)

private:
	// CONFIGURATION
	/** The character configuration data asset. */
	UPROPERTY(BlueprintGetter = GetCharacterConfiguration, EditAnywhere, Category = "Player Character|Configuration", Meta = (DisplayName = "Character Configuration", DisplayPriority = "0"))
	UPlayerCharacterConfiguration* Configuration;
	
	// COMPONENTS
	/** The camera for the player. */
	UPROPERTY(BlueprintGetter = GetCamera, EditAnywhere, Category = "Player Character|Camera", Meta = (DisplayName = "Camera"))
	UCameraComponent* Camera;
	
	/** The CameraController that handles first person camera behavior. */
	UPROPERTY(BlueprintGetter = GetCameraController, EditAnywhere, Category = "Player Character|Components", Meta = (DisplayName = "Camera Controller"))
	UPlayerCameraController* CameraController;
	
	/** The PlayerCharacterMovementComponent that handles the PlayerCharacter's movement. */
	UPROPERTY(BlueprintGetter = GetPlayerCharacterMovement, EditAnywhere, Category = "Player Character|Components", Meta = (DisplayName = "Player Character Movement Component"))
	UPlayerCharacterMovementComponent* PlayerCharacterMovement;
	
	/** The PlayerCharacterController that is currently controlling this PlayerCharacter. */
	UPROPERTY(BlueprintGetter = GetPlayerCharacterController)
	APlayerCharacterController* PlayerCharacterController;

	/** The body collision component for the player. */
	UPROPERTY(EditDefaultsOnly, Category = "Player Character|Body Collision", Meta = (DisplayName = "Body Collision"))
	UPlayerBodyCollisionComponent* BodyCollision;

	/** The foot collision component for the left foot. */
	UPROPERTY(EditDefaultsOnly, Category = "Player Character|Foot Collision", Meta = (DisplayName = "Left Foot Collision"))
	UPlayerFootCollisionComponent* LeftFootCollision;
	
	/** The foot collision component for the right foot. */
	UPROPERTY(EditDefaultsOnly, Category = "Player Character|Foot Collision", Meta = (DisplayName = "Right Foot Collision"))
	UPlayerFootCollisionComponent* RightFootCollision;
	
	// VARIABLES
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

	// TIMERS
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
	UFUNCTION(BlueprintPure, Category = "Player Character", Meta = (DisplayName = "Get Clearance Above Pawn"))
	float GetClearanceAbovePawn() const;
	
	/** Checks whether the player can currently jump. */
	UFUNCTION(BlueprintPure, Category = "Player Character", Meta = (DisplayName = "Can Jump"))
	bool CanPerformJump() const;
	
	/** Checks whether the player can currently enter crouch. */
	bool CanCrouch() const override;
	
	/** Checks whether the player can stand up and stop crouching. */
	UFUNCTION(BlueprintPure, Category = "Player Character", Meta = (DisplayName = "Can Stand Up"))
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
	
	/** Returns a scaled yaw delta value that is used for turn-in-place rotation.
	 *	@Param YawDelta The delta yaw rotation between the player's control rotation and the character's mesh rotation.
	 *	@Param DeltaTime The frame time in seconds.
	 *	@Param Factor A multiplier that affects the rotation speed.
	 *	@Param Clamp The maximum allowed YawDelta angle before the rotation speed should be clamped to prevent the camera from rotation too much in relation to the character's neck. //TODO: This doesn't work properly currently.
	 *	@Return A float value representing a rotator's yaw axis.
	 */
	static float CalculateTurnInPlaceRotation (const float YawDelta, const float DeltaTime, const float Factor, const float Clamp);

	/** Handles the landing callback from the player character movement component. */
	UFUNCTION()
	void HandleLanding(EPlayerLandingType Value);

	/** Handles the ending of a landing. */
	UFUNCTION()
	void HandleLandingEnd();

public:
	/** Returns the Character configuration. */
	UFUNCTION(BlueprintGetter, Category = "Player Character|Configuration", Meta = (DisplayName = "Get Character Configuration"))
	FORCEINLINE UPlayerCharacterConfiguration* GetCharacterConfiguration() const {return Configuration; }
	
	/** Returns the PlayerCharacterController that is controlling this PlayerCharacter. */
	UFUNCTION(BlueprintGetter, Category = "Player Character|Locomotion", Meta = (DisplayName = "PlayerCharacterController"))
	FORCEINLINE APlayerCharacterController* GetPlayerCharacterController() const {return PlayerCharacterController; }
	
	/** Returns the camera component. */
	UFUNCTION(BlueprintGetter, Category = "Player Character|Components", Meta = (DisplayName = "Camera"))
	FORCEINLINE UCameraComponent* GetCamera() const {return Camera; }
	
	/** Returns the Player Camera Controller. */
	UFUNCTION(BlueprintGetter, Category = "Player Character|Components", Meta = (DisplayName = "Camera Controller"))
	FORCEINLINE UPlayerCameraController* GetCameraController() const {return CameraController; }
	
	/** Returns the PlayerCharacterMovementComponent. */
	UFUNCTION(BlueprintGetter, Category = "Player Character|Components", Meta = (DisplayName = "Player Character Movement Component"))
	FORCEINLINE UPlayerCharacterMovementComponent* GetPlayerCharacterMovement() const {return PlayerCharacterMovement; }

	/** Returns the target speed of the player character. */
	UFUNCTION(BlueprintGetter, Category = "Player Character|Locomotion", Meta = (DisplayName = "Target Movement Speed"))
	FORCEINLINE float GetTargetSpeed() const { return TargetSpeed; }

	/** Returns the scaled speed of the player character. */
	UFUNCTION(BlueprintGetter, Category = "Player Character|Locomotion", Meta = (DisplayName = "Scaled Movement Speed"))
	FORCEINLINE float GetScaledSpeed() const { return ScaledSpeed; }
	
	/** Returns if the character is currently turning in place. */
	UFUNCTION(BlueprintGetter, Category = "Player Character|Locomotion", Meta = (DisplayName = "Is Turning In Place"))
	FORCEINLINE bool GetIsTurningInPlace() const { return IsTurningInPlace; }

	/** Returns the character yaw delta between the facing direction of the character and the camera. */
	UFUNCTION(BlueprintGetter, Category = "Player Character|Locomotion", Meta = (DisplayName = "Yaw Delta"))
	FORCEINLINE float GetYawDelta() const { return YawDelta; }

	/** Returns whether the character is currently sprinting. */
	UFUNCTION(BlueprintPure, Category = "Player Character|Locomotion", Meta = (DisplayName = "Is Sprinting"))
	FORCEINLINE bool IsSprinting() const
	{
		if(PlayerCharacterMovement)
		{
			return PlayerCharacterMovement->GetIsSprinting();
		}
		return false;
	}
};

UCLASS(BlueprintType, ClassGroup = "PlayerCharacter")
class FROSTBITE_API UPlayerCharacterConfiguration : public UDataAsset
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogPlayerCharacterConfiguration, Log, All)

public:
	/** Defines the default movement speed.*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "General",
		Meta = (DisplayName = "Default Movement Speed", ForceUnits = "cm/s", ClampMin = "0", ClampMax = "400", UiMin = "0", UIMax = "400"))
	float WalkSpeed {300.f};

	/** The maximum allowed slope angle the player character can climb on. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "General", Meta = (DisplayName = "Max Walkable Floor Angle", Units = "Degrees",
		ClampMin = "0", ClampMax = "60", UIMin = "0", UIMax = "60"))
	float MaxWalkableFloorAngle {35.0f};

	/** The maximum step height the player can perform. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "General", Meta = (DisplayName = "Max Step Height", Units = "Centimeters",
	ClampMin = "0", ClampMax = "50", UIMin = "0", UIMax = "50"))
	float MaxStepHeight {30.0f};
	
	/** When set to enabled, the character is allowed to jump. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Jumping",
		Meta = (DisplayName = "Enable Jumping"))
	bool IsJumpingEnabled {true};

	/** Defines the jump velocity of the character. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Jumping",
		Meta = (DisplayName = "Jump Velocity", ClampMin = "0.0", UIMin = "0.0", ClampMax = "1000.0", UIMax = "1000.0"))
	float JumpVelocity {440.f};

	/** When set to enabled, the character is allowed to sprint. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Sprinting",
		Meta = (DisplayName = "Enable Sprinting"))
	bool IsSprintingEnabled {true};

	/** Defines the movement speed when sprinting. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Sprinting",
		Meta = (DisplayName = "Sprint Speed", ForceUnits = "cm/s", EditCondition= "IsSprintingEnabled", ClampMin = "400", ClampMax = "800", UiMin = "400", UIMax = "800"))
	float SprintSpeed {600.f};

	/** When set to enabled, the character is allowed to crouch. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Crouching",
		Meta = (Displayname = "Enable Crouching"))
	bool IsCrouchingEnabled {true};

	/** Defines the characters movement speed while crouched. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Crouching",
		Meta = (Displayname = "Crouch Speed", ForceUnits = "cm/s", EditCondition= "IsCrouchingEnabled", ClampMin = "0", ClampMax = "300", UiMin = "0", UIMax = "300"))
	float CrouchSpeed {200.f};

	/** When set to enabled, The crouch button will act as a toggle for the crouch action.
	 *	Enable this if you do not want the player to have to hold down the crouch button to remain crouched. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Crouching",
		Meta = (Displayname = "Crouch Toggle Mode", EditCondition= "IsCrouchingEnabled" , AdvancedDisplay = "true"))
	bool EnableCrouchToggle {false};
	
	/** Defines the rotation rate when using a gamepad.*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Camera",
		Meta = (Displayname = "Gamepad Rotation Rate"))
	float RotationRate {150.f};

	/** When true, camera rotation smoothing is applied to the player. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Rotation Smoothing", Meta = (DisplayName = "Enable Rotation Smoothing"))
	bool IsRotationSmoothingEnabled {false};

	/** The rotation smoothing speed. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Rotation Smoothing",
		Meta = (Displayname = "Rotation Smoothing Speed", ClampMin = "0", ClampMax = "20", UiMin = "0", UIMax = "20"))
	float RotationSmoothingSpeed {8};

	/** If true, the player is slowed down when stepping over larger physics bodies. */
	UPROPERTY(BlueprintReadOnly, Category = "Collision|StepOver", Meta = (DisplayName = "Slow Down Character When Stepping Over Physics Bodies"))
	bool IsStepOverEnabled {true};

	/** The minimum weight of an object required to slow down the player. */
	UPROPERTY(BlueprintReadOnly, Category = "Collision|StepOver", Meta = (DisplayName = "Weight Threshold", Units = "Kilograms",
		ClampMin = "0", EditCondition = "IsStepOverEnabled"))
	float StepOverWeightThreshold {10.0f};
	
	/** The minimum size of an object required to slow down the player */
	UPROPERTY(BlueprintReadOnly, Category = "Collision|StepOver", Meta = (DisplayName = "Size Threshold", ClampMin = "0",
	EditCondition = "IsStepOverEnabled"))
	float StepOverSizeThreshold {100.0f};

	/** The base speed modifier to apply when the player is performing step-overs. */
	UPROPERTY(BlueprintReadOnly, Category = "Collision|StepOver", Meta = (DisplayName = "Base Modifier",
		ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1", EditCondition = "IsStepOverEnabled"))
	float StepOverBaseSpeedModifier {0.6};
	
	/** Constructor with default values. */
	UPlayerCharacterConfiguration()
	{
	}
	
	/** Applies the character configuration to a PlayerCharacter instance. */
	void ApplyToPlayerCharacter(const APlayerCharacter* PlayerCharacter);

	/** Applies some values of the character configuration to the player controller and it's corresponding camera manager. */
	void ApplyToPlayerController(APlayerController* PlayerController);
};




