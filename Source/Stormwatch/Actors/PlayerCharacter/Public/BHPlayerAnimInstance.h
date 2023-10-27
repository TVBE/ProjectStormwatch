// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BHStepData.h"
#include "BHPlayerAnimInstance.generated.h"

UENUM(BlueprintType)
enum class EBHBodyExtremity : uint8
{
	LeftFoot,
	RightFoot,
	LeftHand,
	RightHand
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FExtremityHitDelegate, FBHStepData, FootstepData);

UCLASS(Abstract, Blueprintable, BlueprintType, Classgroup = "PlayerCharacter")
class STORMWATCH_API UBHPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UBHPlayerAnimInstance();

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "AnimInstance")
	void GetExtremityData(FBHStepData& StepData, const EBHBodyExtremity BodyPart);

	UPROPERTY(BlueprintAssignable)
	FExtremityHitDelegate OnExtremityHit;

protected:
	// Properties to be evaluated through Fast-Path in the animation state machine. 
	// All data has to be evaluated and copied over to the AnimInstance during runtime.
	UPROPERTY(BlueprintReadWrite)
	bool bIsMovementPending = false;

	UPROPERTY(BlueprintReadWrite)
	bool bIsMoving = false;

	UPROPERTY(BlueprintReadWrite)
	bool bIsMovingLongitudinally = false;

	UPROPERTY(BlueprintReadWrite)
	bool bIsMovingLaterally = false;

	UPROPERTY(BlueprintReadWrite)
	bool bStopLongitudinalMovement = false;

	UPROPERTY(BlueprintReadWrite)
	bool bIsTurningLeft = false;

	UPROPERTY(BlueprintReadWrite)
	bool bIsTurningRight = false;

	UPROPERTY(BlueprintReadWrite)
	bool bIsJumping = false;

	UPROPERTY(BlueprintReadWrite)
	bool bIsFalling = false;

	UPROPERTY(BlueprintReadWrite)
	bool bIsAirborne = false;

	UPROPERTY(BlueprintReadWrite)
	bool bDoHardLanding = false;

	UPROPERTY(BlueprintReadWrite)
	bool bDoHeavyLanding = false;

	UPROPERTY(BlueprintReadWrite)
	bool bIsSprinting = false;

	UPROPERTY(BlueprintReadWrite)
	bool bDoSprintStart = false;

	UPROPERTY(BlueprintReadWrite)
	bool bDoSprintSop = false;

	UPROPERTY(BlueprintReadWrite)
	bool bIsCrouching = false;

	UPROPERTY(BlueprintReadWrite)
	float Speed = 0.0f;

	UPROPERTY(BlueprintReadWrite)
	float TurnSpeed = 0.0f;

	UPROPERTY(BlueprintReadWrite)
	float Direction = 0.0f;

	UPROPERTY(BlueprintReadWrite)
	float FallTime = 0.0f;

	UPROPERTY(BlueprintReadWrite)
	float VerticalAlpha = 0.0f;

private:
	void UpdateFallTime(const float DeltaTime);

	void CheckMovementState(const class ABHPlayerCharacterController& Controller, const class UBHPlayerMovementComponent& CharacterMovement);
	void CheckTurnInplaceConditions(const ABHPlayerCharacter& Character);

	static float GetSpeed(const ABHPlayerCharacter& Character, const UBHPlayerMovementComponent& CharacterMovement);
	static float GetDirection(const ABHPlayerCharacter& Character);
};
