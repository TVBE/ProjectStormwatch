// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerMovementComponent.generated.h"

UENUM(BlueprintType)
enum class EPlayerGroundMovementType : uint8
{
	Idle				UMETA(DisplayName = "Idle"),
	Walking				UMETA(DisplayName = "Walking"),
	Sprinting			UMETA(DisplayName = "Sprinting"),
};

UENUM(BlueprintType)
enum class EPlayerLandingType : uint8
{
	Soft				UMETA(DisplayName = "Soft Landing"),
	Hard				UMETA(DisplayName = "Hard Landing"),
	Heavy				UMETA(DisplayName = "Heavy Landing"),
};

UENUM(BlueprintType)
enum class EPlayerLocomotionEvent : uint8
{
	Jump				UMETA(DisplayName = "Jump"),
	Fall				UMETA(DisplayName = "Fall"),
	CrouchStart			UMETA(DisplayName = "Start Crouching"),
	CrouchEnd			UMETA(DisplayName = "Stop Crouching"),
	SprintStart			UMETA(DisplayName = "Start Sprinting"),
	SprintEnd			UMETA(DisplayName = "Stop Sprinting")
};

USTRUCT(BlueprintType)
struct FPlayerCharacterMovementSettings
{
	GENERATED_USTRUCT_BODY()

	/** Defines the default movement speed.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "General",
			  Meta = (DisplayName = "Default Movement Speed", ForceUnits = "cm/s",
					  ClampMin = "0", ClampMax = "400", UiMin = "0", UIMax = "400"))
	float WalkSpeed = 300.f;

	/** The maximum allowed slope angle the player character can climb on. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "General",
			  Meta = (Units = "Degrees", ClampMin = "0", ClampMax = "60", UIMin = "0", UIMax = "60"))
	float MaxWalkableFloorAngle = 35.0f;

	/** The maximum step height the player can perform. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "General",
			  Meta = (Units = "Centimeters", ClampMin = "0", ClampMax = "50", UIMin = "0", UIMax = "50"))
	float MaxStepHeight = 30.0f;

	/** When set to enabled, the character is allowed to jump. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jumping",
			  Meta = (DisplayName = "Enable Jumping"))
	bool JumpingEnabled = true;

	/** Defines the jump velocity of the character. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jumping",
			  Meta = (ClampMin = "0", ClampMax = "1000", UIMin = "0", UIMax = "1000",
					  EditCondition = "JumpingEnabled"))
	float JumpVelocity = 440.f;

	/** When set to enabled, the character is allowed to sprint. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sprinting",
			  Meta = (DisplayName = "Enable Sprinting"))
	bool SprintingEnabled = true;

	/** Defines the movement speed when sprinting. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sprinting",
			  Meta = (ForceUnits = "cm/s", EditCondition = "SprintingEnabled",
					  ClampMin = "400", ClampMax = "800", UiMin = "400", UIMax = "800"))
	float SprintSpeed = 600.f;

	/** When set to enabled, the character is allowed to crouch. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crouching",
			  Meta = (Displayname = "Enable Crouching"))
	bool CrouchingEnabled = true;

	/** Defines the characters movement speed while crouched. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crouching",
			  Meta = (ForceUnits = "cm/s", EditCondition = "CrouchingEnabled",
					  ClampMin = "0", ClampMax = "300", UiMin = "0", UIMax = "300"))
	float CrouchSpeed = 200.f;

	/** Defines the crouch toggle mode. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crouching",
			  Meta = (EditCondition = "CrouchingEnabled", AdvancedDisplay = "true"))
	ECrouchToggleMode CrouchToggleMode = ECrouchToggleMode::PressAndHold;

	/** Defines the rotation rate when using a gamepad.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera",
			  Meta = (Displayname = "Gamepad Rotation Rate"))
	float RotationRate = 150.f;

	/** When true, camera rotation smoothing is applied to the player. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rotation Smoothing",
			  Meta = (DisplayName = "Enable Rotation Smoothing"))
	bool RotationSmoothingEnabled = false;

	/** The rotation smoothing speed. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rotation Smoothing",
			  Meta = (ClampMin = "0", ClampMax = "20", UiMin = "0", UIMax = "20"))
	float RotationSmoothingSpeed = 8.0f;

	/** The Weight range in which we apply interaction rotation scaling.
	 *  X defines the minimum amount of weight before we start fading in the scaling.
	 *  Y defines the maximum amount of weight for when we start applying maximum scaling. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Rotation|Weight",
			  Meta = (DisplayName = "Weight Range",
					  ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100"))
	FVector2D InteractionRotationWeightRange = FVector2D(8, 25);

	/** The Weight scaling range. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Rotation|Weight",
			  Meta = (DisplayName = "Weight Scalars",
					  ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	FVector2D InteractionRotationWeightScalars = FVector2D(1.0, 0.4);

	/** The Bounding box range in which we apply interaction rotation scaling. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Rotation|Size",
			  Meta = (DisplayName = "Size Range",
					  ClampMin = "0", ClampMax = "2000000", UIMin = "0", UIMax = "2000000"))
	FVector2D InteractionRotationSizeRange = FVector2D(500000, 1000000);

	/** The Bounding box scaling range. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Rotation|Size",
			  Meta = (DisplayName = "Size Scalars",
					  ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	FVector2D InteractionRotationSizeScalars = FVector2D(1.0, 0.4);

	/** The Object distance range in which we apply interaction rotation scaling. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Rotation|Distance",
			  Meta = (DisplayName = "Distance Range",
					  ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "200"))
	FVector2D InteractionRotationDistanceRange = FVector2D(25, 150);

	/** The distance scaling range. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Rotation|Distance",
			  Meta = (DisplayName = "Size Scalars",
					  ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	FVector2D InteractionRotationDistanceScalars = FVector2D(1.0, 0.5);

	/** The offset range in which we apply interaction rotation scaling. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Rotation|Offset",
			  Meta = (DisplayName = "Offset Range",
					  ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "200"))
	FVector2D InteractionRotationOffsetRange = FVector2D(8, 20);

	/** The offset scaling range. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Rotation|Offset",
			  Meta = (DisplayName = "Offset Scalars",
					  ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	FVector2D InteractionRotationOffsetScalars = FVector2D(1.0, 0.1);

	/** The Scaling floor for weight and bounding box sized combined. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Rotation ",
			  Meta = (DisplayName = "Multiplier Floor",
					  ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	float InteractionRotationFloor = 0.4;

	/** The Weight range in which we apply interaction speed scaling. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Speed|Weight",
			  Meta = (DisplayName = "Weight Range",
					  ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100"))
	FVector2D InteractionSpeedWeightRange = FVector2D(8, 25);

	/** The Weight scaling range. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Speed|Weight",
			  Meta = (DisplayName = "Weight Scalars",
					  ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	FVector2D InteractionSpeedWeightScalars = FVector2D(1.0, 0.6);

	/** The Bounding box range in which we apply interaction speed scaling. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Speed|Size",
			  Meta = (DisplayName = "Size Range",
					  ClampMin = "0", ClampMax = "2000000", UIMin = "0", UIMax = "2000000"))
	FVector2D InteractionSpeedSizeRange = FVector2D(500000, 1000000);

	/** The Bounding box scaling range. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Speed|Size",
			  Meta = (DisplayName = "Size Scalars",
					  ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	FVector2D InteractionSpeedSizeScalars = FVector2D(1.0, 0.6);

	/** The Scaling floor for weight and bounding box sized combined. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Speed ",
			  Meta = (DisplayName = "Multiplier Floor",
					  ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	float InteractionSpeedFloor = 0.6;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLocomotionEventDelegate, EPlayerLocomotionEvent, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLandingDelegate, EPlayerLandingType, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FJumpDelegate);

UCLASS(ClassGroup = "PlayerCharacter", Meta = (DisplayName = "Player Character Movement Component"))
class STORMWATCH_API UPlayerMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UPlayerMovementComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void UpdateMovementSpeed();

	virtual bool DoJump(bool bReplayingMoves) override;

	EPlayerGroundMovementType GetGroundMovementType() const;

	UFUNCTION()
	void SetIsSprinting(bool bValue);

	UFUNCTION(BlueprintGetter)
	bool GetIsSprinting() const { return IsSprinting; }

	UFUNCTION(BlueprintGetter)
	bool GetIsJumping() const { return IsJumping; }

	float GetTargetSpeed() const { return TargetSpeed; }
	float GetScaledSpeed() const { return ScaledSpeed; }

	UPROPERTY(BlueprintAssignable, Meta = (DisplayName = "Locomotion Event"))
	FLocomotionEventDelegate OnLocomotionEvent;

	UPROPERTY(BlueprintAssignable, Meta = (DisplayName = "Jump Event"))
	FJumpDelegate OnJump;
	
	UPROPERTY(BlueprintAssignable, Meta = (DisplayName = "Landing Event"))
	FLandingDelegate OnLanding;

protected:
	virtual void BeginPlay() override;

	virtual void ProcessLanded(const FHitResult& Hit, float remainingTime, int32 Iterations) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	FPlayerCharacterMovementSettings Settings {};

private:
	float TargetSpeed = 0.0f;
	float ScaledSpeed = 0.0f;

	UPROPERTY(BlueprintGetter = GetIsSprinting)
	bool IsSprinting = false;

	UPROPERTY(BlueprintGetter = GetIsJumping)
	bool IsJumping = false;
};
