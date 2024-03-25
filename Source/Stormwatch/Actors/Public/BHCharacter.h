// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BHCharacter.generated.h"

UENUM(BlueprintType)
enum class EBHPlayerLandingType : uint8
{
	Soft				UMETA(DisplayName = "Soft Landing"),
	Hard				UMETA(DisplayName = "Hard Landing"),
	Heavy				UMETA(DisplayName = "Heavy Landing"),
};

UENUM(BlueprintType)
enum class EBHCrouchToggleMode : uint8
{
	PressAndHold	UMETA(DisplayName = "Press And Hold"),
	Toggle			UMETA(DisplayName = "Toggle")
};

USTRUCT(BlueprintType)
struct FBHCharacterSetup
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "General",
	Meta = (ForceUnits = "cm/s", ClampMin = "0", ClampMax = "400", UIMin = "0", UIMax = "400"))
	float WalkSpeed = 300.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "General",
	Meta = (Units = "Centimeters", ClampMin = "0", ClampMax = "50", UIMin = "0", UIMax = "50"))
	float MaxStepHeight = 30.0f; // NOTE(TV): What is this used for?
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jumping", Meta = (DisplayName = "Enable Jumping"))
	bool bJumpingEnabled = true;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jumping",
	Meta = (EditCondition = "bJumpingEnabled", ClampMin = "0", UIMin = "0"))
	float JumpVelocity = 440.0f;

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
	float SprintSpeed = 600.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crouching", Meta = (Displayname = "Enable Crouching"))
	bool bCrouchingEnabled = true;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crouching",
	Meta = (EditCondition = "bCrouchingEnabled", ForceUnits = "cm/s", ClampMin = "0", ClampMax = "300",
			UIMin = "0", UIMax = "300"))
	float CrouchSpeed = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crouching",
	Meta = (EditCondition = "bCrouchingEnabled", Units = "Centimeters", ClampMin = "0", ClampMax = "300",
		UIMin = "0", UIMax = "300"))
	float UncrouchClearance = 150.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crouching",
	Meta = (EditCondition = "bCrouchingEnabled", AdvancedDisplay = "true"))
	EBHCrouchToggleMode CrouchToggleMode = EBHCrouchToggleMode::PressAndHold;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crouching",
	Meta = (EditCondition = "bCrouchingEnabled", Displayname = "Enable Crouch Sprinting"))
	bool bCrouchSprintingEnabled = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crouching",
	Meta = (EditCondition = "bCrouchSprintingEnabled", ForceUnits = "cm/s", ClampMin = "0", ClampMax = "300",
		UIMin = "0", UIMax = "300"))
	float CrouchSprintSpeed = 250.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera",
	Meta = (Displayname = "Gamepad Rotation Rate"))
	float RotationRate = 150.0f; // TODO (TV): Deprecate this. It is causing more troubles than it's worth.
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rotation Smoothing",
	Meta = (DisplayName = "Enable Rotation Smoothing"))
	bool RotationSmoothingEnabled = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rotation Smoothing",
	Meta = (ClampMin = "0", ClampMax = "20", UIMin = "0", UIMax = "20"))
	float RotationSmoothingSpeed = 8.0f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnJumpSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLandSignature, FHitResult, Hit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCrouchSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnCrouchSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSprintSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndSprintSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "Barrelhouse")
class STORMWATCH_API ABHCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABHCharacter();
	ABHCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaTime) override;

	virtual void Jump() override;
	virtual void Landed(const FHitResult& Hit) override;
	virtual void Crouch(bool bClientSimulation) override;
	virtual void UnCrouch(bool bClientSimulation) override;
	virtual bool CanCrouch() const override;
	
	UFUNCTION(BlueprintPure, Category = "BHCharacter")
	virtual bool CanUncrouch() const;

	UFUNCTION(BlueprintPure, Category = "BHCharacter")
	virtual bool IsCrouched() const;

	UFUNCTION(BlueprintCallable, Category = "BHCharacter")
	virtual void Sprint();

	UFUNCTION(BlueprintCallable, Category = "BHCharacter")
	virtual void EndSprint();

	UFUNCTION(BlueprintPure, Category = "BHCharacter")
	virtual bool CanSprint() const;

	UFUNCTION(BlueprintPure, Category = "BHCharacter")
	virtual bool CanEndSprint() const;

	UFUNCTION(BlueprintPure, Category = "BHCharacter")
	virtual bool IsSprinting() const;

	UFUNCTION(BlueprintCallable, Category = "BHCharacter")
	virtual void Die();

	UFUNCTION(BlueprintPure, Category = "BHCharacter")
	virtual bool CanDie() const;

	UFUNCTION(BlueprintPure, Category = "BHCharacter")
	virtual bool IsDead() const;
	
	UFUNCTION(BlueprintPure, Category = "BHCharacter")
	float GetClearanceAbovePawn(float TraceLength = 500.0f) const;

	UFUNCTION(BlueprintPure, Category = "BHCharacter")
	float GetSpeed() const;

	UFUNCTION(BlueprintPure, Category = "BHCharacter")
	class UBHCharacterMovementComponent* GetCharacterMovementComponent() const;

	UFUNCTION(BlueprintGetter, Category = "BHCharacter")
	const FBHCharacterSetup& GetSetup() const;
	FBHCharacterSetup& GetSetup();

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnJumpSignature OnJump;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnLandSignature OnLand;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnCrouchSignature OnCrouch;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnUnCrouchSignature OnUnCrouch;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnSprintSignature OnSprint;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnEndSprintSignature OnEndSprint;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnDeathSignature OnDeath;
	
private:
	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetSetup, Meta = (ShowOnlyInnerProperties))
	FBHCharacterSetup Setup {};
	
	UPROPERTY(Transient)
	bool bSprinting = false;

	UPROPERTY(Transient)
	bool bDead = false;
	
	UPROPERTY(Transient)
	float Speed = 0.0f;
};
