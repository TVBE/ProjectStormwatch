// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "StepData.h"
#include "PlayerAnimInstance.generated.h"

class APlayerCharacter;
class APlayerCharacterController;
class UPlayerMovementComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFootstepDelegate, FStepData, FootstepData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHandstepDelegate, FStepData, HandstepData);

/** The AnimInstance class is an instance of an animation asset that can be played on a skeletal mesh.
 *	This class is implemented as an Animation Blueprint, with most logic being executed through Blueprint nodes.
 *	We mainly declare functions here to be used a BlueprintCallable UFunctions.
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class STORMWATCH_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPlayerAnimInstance();

	virtual void NativeInitializeAnimation() override;
	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	/** Returns data about a footstep at the specified foot, like the object or physical material underneath the foot at the time of the footstep.
	 *	@param Foot The foot that is performing the step.
	 *	@return StepData structure containing relevant information about the location and velocity of the foot at the time of the footstep.
	 */
	UFUNCTION(BlueprintPure)
	FStepData GetFootstepData(const ELeftRight Foot);

	/** Returns data about a 'handstep' at the specified hand, like the object or physical material underneath the hand at the time of the handstep.
	*	@param Hand The hand that is performing the step.
	*	@return StepData structure containing relevant information about the location and velocity of the hand at the time of the handstep.
	*/
	UFUNCTION(BlueprintPure)
	FStepData GetHandstepData(const ELeftRight Hand);

	void GetStepData(FStepData& StepData, const FVector Location);

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FFootstepDelegate OnFootstep;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FFootstepDelegate OnHandstep;

protected:
	/** Booleans to be evaluated through Fast-Path in the animation state machine. All data has to be evaluated and copied over to the AnimInstance during runtime. */
	UPROPERTY(BlueprintReadWrite)
	bool IsMovementPending {false};

	UPROPERTY(BlueprintReadWrite)
	bool IsMoving {false};

	UPROPERTY(BlueprintReadWrite)
	bool IsMovingLongitudinally {false};

	UPROPERTY(BlueprintReadWrite)
	bool IsMovingLaterally {false};

	UPROPERTY(BlueprintReadWrite)
	bool StopLongitudinalMovement {false};

	UPROPERTY(BlueprintReadWrite)
	bool IsTurningLeft {false};

	UPROPERTY(BlueprintReadWrite)
	bool IsTurningRight {false};

	UPROPERTY(BlueprintReadWrite)
	bool IsJumping {false};

	UPROPERTY(BlueprintReadWrite)
	bool IsFalling {false};

	UPROPERTY(BlueprintReadWrite)
	bool IsAirborne {false};

	UPROPERTY(BlueprintReadWrite)
	bool DoHardLanding {false};

	UPROPERTY(BlueprintReadWrite)
	bool DoHeavyLanding {false};

	UPROPERTY(BlueprintReadWrite)
	bool IsSprinting {false};

	UPROPERTY(BlueprintReadWrite)
	bool DoSprintStart {false};

	UPROPERTY(BlueprintReadWrite)
	bool DoSprintSop {false};

	UPROPERTY(BlueprintReadWrite)
	bool IsCrouching {false};

	UPROPERTY(BlueprintReadWrite)
	float Speed {0.0f};

	UPROPERTY(BlueprintReadWrite)
	float TurnSpeed {0.0f};

	UPROPERTY(BlueprintReadWrite)
	float Direction {0.0f};

	UPROPERTY(BlueprintReadWrite)
	float FallTime {0.0f};

	UPROPERTY(BlueprintReadWrite)
	float VerticalAlpha {0.0f};

private:
	/** Checks the movement state of the character and updates certain state machine conditions. */
	void CheckMovementState(const APlayerCharacter& Character, const APlayerCharacterController& Controller, const UPlayerMovementComponent& CharacterMovement);

	/** Checks whether the character is turning in place, and updates certain state machine conditions accordingly. */
	void CheckTurnInplaceConditions(const APlayerCharacter& Character);

	/** Returns the direction the character is moving in. */
	static float GetDirection(const APlayerCharacter& Character);

	/** Returns the speed that the character is moving at. */
	static float GetSpeed(const APlayerCharacter& Character, const UPlayerMovementComponent& CharacterMovement);

	/** Updates the time the player is falling, if the player is falling. */
	void UpdateFallTime(const float DeltaTime);
};
