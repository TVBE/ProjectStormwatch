// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BHPlayerMovementComponent.generated.h"

UENUM(BlueprintType)
enum class EBHPlayerLandingType : uint8
{
	Soft				UMETA(DisplayName = "Soft Landing"),
	Hard				UMETA(DisplayName = "Hard Landing"),
	Heavy				UMETA(DisplayName = "Heavy Landing"),
};

UENUM(BlueprintType)
enum class EBHPlayerLocomotionEvent : uint8
{
	Jump				UMETA(DisplayName = "Jump"),
	Fall				UMETA(DisplayName = "Fall"),
	CrouchStart			UMETA(DisplayName = "Start Crouching"),
	CrouchEnd			UMETA(DisplayName = "Stop Crouching"),
	SprintStart			UMETA(DisplayName = "Start Sprinting"),
	SprintEnd			UMETA(DisplayName = "Stop Sprinting")
};

UENUM(BlueprintType)
enum class EBHCrouchToggleMode : uint8
{
	PressAndHold	UMETA(DisplayName = "Press And Hold"),
	Toggle			UMETA(DisplayName = "Toggle")
};

USTRUCT(BlueprintType)
struct FBHPlayerInteractionMovementModifierSetup
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Rotation|Weight",
	Meta = (DisplayName = "Weight Range", ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100"))
	FVector2D InteractionRotationWeightRange = FVector2D(8, 25);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Rotation|Weight",
	Meta = (DisplayName = "Weight Scalars", ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	FVector2D InteractionRotationWeightScalars = FVector2D(1.0, 0.4);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Rotation|Size",
	Meta = (DisplayName = "Size Range", ClampMin = "0", ClampMax = "2000000", UIMin = "0", UIMax = "2000000"))
	FVector2D InteractionRotationSizeRange = FVector2D(500000, 1000000);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Rotation|Size",
	Meta = (DisplayName = "Size Scalars", ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	FVector2D InteractionRotationSizeScalars = FVector2D(1.0, 0.4);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Rotation|Distance",
	Meta = (DisplayName = "Distance Range", ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "200"))
	FVector2D InteractionRotationDistanceRange = FVector2D(25, 150);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Rotation|Distance",
	Meta = (DisplayName = "Size Scalars", ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	FVector2D InteractionRotationDistanceScalars = FVector2D(1.0, 0.5);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Rotation|Offset",
	Meta = (DisplayName = "Offset Range", ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "200"))
	FVector2D InteractionRotationOffsetRange = FVector2D(8, 20);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Rotation|Offset",
	Meta = (DisplayName = "Offset Scalars", ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	FVector2D InteractionRotationOffsetScalars = FVector2D(1.0, 0.1);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Rotation ",
	Meta = (DisplayName = "Multiplier Floor", ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	float InteractionRotationFloor = 0.4;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Speed|Weight",
	Meta = (DisplayName = "Weight Range", ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100"))
	FVector2D InteractionSpeedWeightRange = FVector2D(8, 25);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Speed|Weight",
	Meta = (DisplayName = "Weight Scalars", ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	FVector2D InteractionSpeedWeightScalars = FVector2D(1.0, 0.6);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Speed|Size",
	Meta = (DisplayName = "Size Range", ClampMin = "0", ClampMax = "2000000", UIMin = "0", UIMax = "2000000"))
	FVector2D InteractionSpeedSizeRange = FVector2D(500000, 1000000);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Speed|Size",
	Meta = (DisplayName = "Size Scalars", ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	FVector2D InteractionSpeedSizeScalars = FVector2D(1.0, 0.6);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Speed ",
	Meta = (DisplayName = "Multiplier Floor", ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	float InteractionSpeedFloor = 0.6;
};

USTRUCT(BlueprintType)
struct FBHPlayerCharacterMovementSetup
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "General",
	Meta = (ForceUnits = "cm/s", ClampMin = "0", ClampMax = "400", UIMin = "0", UIMax = "400"))
	float WalkSpeed = 300.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "General",
	Meta = (Units = "Centimeters", ClampMin = "0", ClampMax = "50", UIMin = "0", UIMax = "50"))
	float MaxStepHeight = 30.0f; // NOTE(TV): What is this used for?
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jumping", Meta = (DisplayName = "Enable Jumping"))
	bool bJumpingEnabled = true;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jumping",
	Meta = (EditCondition = "bJumpingEnabled", ClampMin = "0", UIMin = "0"))
	float JumpVelocity = 440.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jumping",
	Meta = (EditCondition = "bJumpingEnabled", InlineEditConditionToggle))
	bool bRequireJumpClearance = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jumping",
	Meta = (EditCondition = "bJumpingEnabled && bRequireJumpClearance", Units = "Centimeters",
			ClampMin = "0", UIMin = "0"))
	float JumpClearance = 80.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sprinting",
	Meta = (DisplayName = "Enable Sprinting"))
	bool bSprintingEnabled = true;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sprinting",
	Meta = (ForceUnits = "cm/s", EditCondition = "bSprintingEnabled", ClampMin = "400", ClampMax = "800",
			UIMin = "400", UIMax = "800"))
	float SprintSpeed = 600.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crouching", Meta = (Displayname = "Enable Crouching"))
	bool bCrouchingEnabled = true;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crouching",
	Meta = (EditCondition = "bCrouchingEnabled", ForceUnits = "cm/s", ClampMin = "0", ClampMax = "300",
			UIMin = "0", UIMax = "300"))
	float CrouchSpeed = 200.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crouching",
	Meta = (EditCondition = "bCrouchingEnabled", AdvancedDisplay = "true"))
	EBHCrouchToggleMode CrouchToggleMode = EBHCrouchToggleMode::PressAndHold;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crouching",
	Meta = (EditCondition = "bCrouchingEnabled", Displayname = "Enable Crouch Sprinting"))
	bool bCrouchSprintingEnabled = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crouching",
	Meta = (EditCondition = "bCrouchSprintingEnabled", ForceUnits = "cm/s", ClampMin = "0", ClampMax = "300",
		UIMin = "0", UIMax = "300"))
	float CrouchSprintSpeed = 250.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera",
	Meta = (Displayname = "Gamepad Rotation Rate"))
	float RotationRate = 150.f; // TODO (TV): Deprecate this. It is causing more troubles than it's worth.
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rotation Smoothing",
	Meta = (DisplayName = "Enable Rotation Smoothing"))
	bool RotationSmoothingEnabled = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rotation Smoothing",
	Meta = (ClampMin = "0", ClampMax = "20", UIMin = "0", UIMax = "20"))
	float RotationSmoothingSpeed = 8.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	FBHPlayerInteractionMovementModifierSetup InteractionModifiers {};
};

USTRUCT(BlueprintType)
struct FBHGroundSpeedData
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(BlueprintReadOnly, Category = "Ground Speed")
	float BaseSpeed = 0.0f;
	
	UPROPERTY(BlueprintReadOnly, Category = "Ground Speed")
	float ScaledSpeed = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Ground Speed")
	float Multiplier = 1.0f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FJumpDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLandingDelegate, EBHPlayerLandingType, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSprintStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSprintStop);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCrouchStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCrouchStop);

UCLASS(Notblueprintable, Blueprinttype, ClassGroup = "BHPlayerCharacter")
class STORMWATCH_API UBHPlayerMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UBHPlayerMovementComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void UpdateWalkSpeed(float DeltaTime);
	
	UFUNCTION(BlueprintPure, Category = "BHPlayerMovement")
	const FBHPlayerCharacterMovementSetup& GetMovementSetup() const { return MovementSetup;  }
	
	UFUNCTION(BlueprintCallable, Category = "BHPlayerMovement")
	void Jump();

	UFUNCTION(BlueprintPure, Category = "BHPlayerMovementComponent")
	bool CanJump() const;

	UFUNCTION(BlueprintPure, Category = "BHPlayerMovementComponent")
	bool IsJumping() const { return bIsJumping; }

	UFUNCTION(BlueprintCallable, Category = "BHPlayerMovementComponent")
	void StartSprinting();

	UFUNCTION(BlueprintCallable, Category = "BHPlayerMovementComponent")
	void StopSprinting();

	UFUNCTION(BlueprintPure, Category = "BHPlayerMovementComponent")
	bool IsSprinting() const;

	UFUNCTION(BlueprintPure, Category = "BHPlayerMovementComponent")
	bool CanSprint() const;

	UFUNCTION(BlueprintCallable, Category = "BHPlayerMovementComponent")
	void StartCrouching();

	UFUNCTION(BlueprintCallable, Category = "BHPlayerMovementComponent")
	void StopCrouching();

	virtual bool IsCrouching() const override;

	UFUNCTION(BlueprintPure, Category = "BHPlayerMovementComponent")
	bool CanCrouch() const;

	UFUNCTION(BlueprintPure, Category = "BHPlayerMovementComponent")
	bool CanUncrouch() const;
	
	UPROPERTY(BlueprintAssignable, Category = "BHPlayerMovementComponent|Delegates")
	FOnSprintStart OnSprintStart;

	UPROPERTY(BlueprintAssignable, Category = "BHPlayerMovementComponent|Delegates")
	FOnSprintStop OnSprintStop;
	
	UPROPERTY(BlueprintAssignable, Category = "BHPlayerMovementComponent|Delegates")
	FOnCrouchStart OnCrouchStart;
	
	UPROPERTY(BlueprintAssignable, Category = "BHPlayerMovementComponent|Delegates")
	FOnCrouchStop OnCrouchStop;
	
private:
	virtual void ProcessLanded(const FHitResult& Hit, float remainingTime, int32 Iterations) override;
	
	float GetWalkSpeedMultiplier() const;
	
	FBHGroundSpeedData WalkSpeedData {};
	
	bool bSprinting = false;
	bool bCrouching = false;
	
	UPROPERTY(BlueprintAssignable, Meta = (DisplayName = "Jump Event"))
	FJumpDelegate OnJump;
	
	UPROPERTY(BlueprintAssignable, Meta = (DisplayName = "Landing Event"))
	FLandingDelegate OnLanding;
	
	class ABHPlayerCharacter* GetPlayerCharacter() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess = "true"))
	FBHPlayerCharacterMovementSetup MovementSetup {};
	
	bool bIsSprinting = false;
	bool bIsJumping = false;
};
