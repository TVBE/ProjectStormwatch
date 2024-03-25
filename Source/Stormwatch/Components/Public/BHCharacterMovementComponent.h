// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BHCharacterMovementComponent.generated.h"

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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLandedSignature, EBHPlayerLandingType, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSprintStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSprintStop);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCrouchStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCrouchStop);

UCLASS(Notblueprintable, Blueprinttype, ClassGroup = "BHPlayerCharacter")
class STORMWATCH_API UBHCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UBHCharacterMovementComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void UpdateWalkSpeed(float DeltaTime);
	
	UFUNCTION(BlueprintPure, Category = "BHPlayerMovement")
	const FBHCharacterSetup& GetMovementSetup() const { return MovementSetup;  }
	
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

	UPROPERTY(Transient)
	bool bWantsToSprint = false;
	
private:
	virtual void ProcessLanded(const FHitResult& Hit, float remainingTime, int32 Iterations) override;
	
	float GetWalkSpeedMultiplier() const;
	
	FBHGroundSpeedData WalkSpeedData {};
	
	bool bSprinting = false;
	bool bCrouching = false;
	
	UPROPERTY(BlueprintAssignable, Meta = (DisplayName = "Jump Event"))
	FJumpDelegate OnJump;
	
	UPROPERTY(BlueprintAssignable, Meta = (DisplayName = "Landing Event"))
	FOnLandedSignature OnLanding;
	
	class ABHPlayerCharacter* GetPlayerCharacter() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess = "true"))
	FBHCharacterSetup MovementSetup {};
	
	bool bIsSprinting = false;
	bool bIsJumping = false;
};
