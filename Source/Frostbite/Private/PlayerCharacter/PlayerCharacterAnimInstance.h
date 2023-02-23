// Copyright 2023 Barrelhouse

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "FootstepData.h"
#include "PlayerCharacterAnimInstance.generated.h"

class APlayerCharacter;
class APlayerCharacterController;
class UPlayerCharacterMovementComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFootstepDelegate, FFootstepData, FootstepData);

/** The AnimInstance class is an instance of an animation asset that can be played on a skeletal mesh.
 *	This class is implemented as an Animation Blueprint, with most logic being executed through Blueprint nodes.
 *	We mainly declare functions here to be used a BlueprintCallable UFunctions.
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class UPlayerCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FFootstepDelegate OnFootstep;

protected:
	/** Booleans to be evaluated through Fast-Path in the animation state machine. All data has to be evaluated and copied over to the AnimInstance during runtime. */
	UPROPERTY(BlueprintReadWrite, Category = "PlayerCharacterAnimInstance|StateMachine", Meta = (DisplayName = "Is Movement Pending"))
	bool IsMovementPending {false};

	UPROPERTY(BlueprintReadWrite, Category = "PlayerCharacterAnimInstance|StateMachine", Meta = (DisplayName = "Is Moving"))
	bool IsMoving {false};
	
	UPROPERTY(BlueprintReadWrite, Category = "PlayerCharacterAnimInstance|StateMachine", Meta = (DisplayName = "Is Moving Longitudinally"))
	bool IsMovingLongitudinally {false};

	UPROPERTY(BlueprintReadWrite, Category = "PlayerCharacterAnimInstance|StateMachine", Meta = (DisplayName = "Is Moving Laterally"))
	bool IsMovingLaterally {false};

	UPROPERTY(BlueprintReadWrite, Category = "PlayerCharacterAnimInstance|StateMachine", Meta = (DisplayName = "Stop Longitudinal Movement"))
	bool StopLongitudinalMovement {false};
	
	UPROPERTY(BlueprintReadWrite, Category = "PlayerCharacterAnimInstance|StateMachine", Meta = (DisplayName = "Is Turning Left"))
	bool IsTurningLeft {false};
	
	UPROPERTY(BlueprintReadWrite, Category = "PlayerCharacterAnimInstance|StateMachine", Meta = (DisplayName = "Is Turning Right"))
	bool IsTurningRight {false};

	UPROPERTY(BlueprintReadWrite, Category = "PlayerCharacterAnimInstance|StateMachine", Meta = (DisplayName = "Is Jumping"))
	bool IsJumping {false};

	UPROPERTY(BlueprintReadWrite, Category = "PlayerCharacterAnimInstance|StateMachine", Meta = (DisplayName = "Is Falling"))
	bool IsFalling {false};

	UPROPERTY(BlueprintReadWrite, Category = "PlayerCharacterAnimInstance|StateMachine", Meta = (DisplayName = "Is Airborne"))
	bool IsAirborne {false};

	UPROPERTY(BlueprintReadWrite, Category = "PlayerCharacterAnimInstance|StateMachine", Meta = (DisplayName = "Do Hard Landing"))
	bool DoHardLanding {false};

	UPROPERTY(BlueprintReadWrite, Category = "PlayerCharacterAnimInstance|StateMachine", Meta = (DisplayName = "Do Heavy Landing"))
	bool DoHeavyLanding {false};

	UPROPERTY(BlueprintReadWrite, Category = "PlayerCharacterAnimInstance|StateMachine", Meta = (DisplayName = "Is Sprinting"))
	bool IsSprinting {false};
	
	UPROPERTY(BlueprintReadWrite, Category = "PlayerCharacterAnimInstance|StateMachine", Meta = (DisplayName = "Do Sprint Start"))
	bool DoSprintStart {false};

	UPROPERTY(BlueprintReadWrite, Category = "PlayerCharacterAnimInstance|StateMachine", Meta = (DisplayName = "Do Sprint Stop"))
	bool DoSprintSop {false};

	UPROPERTY(BlueprintReadWrite, Category = "PlayerCharacterAnimInstance|StateMachine", Meta = (DisplayName = "Speed"))
	float Speed {0.0f};
	
	UPROPERTY(BlueprintReadWrite, Category = "PlayerCharacterAnimInstance|StateMachine", Meta = (DisplayName = "Turn Speed"))
	float TurnSpeed {0.0f};

	UPROPERTY(BlueprintReadWrite, Category = "PlayerCharacterAnimInstance|StateMachine", Meta = (DisplayName = "Direction"))
	float Direction {0.0f};

	UPROPERTY(BlueprintReadWrite, Category = "PlayerCharacterAnimInstance|StateMachine", Meta = (DisplayName = "Fall Time"))
	float FallTime {0.0f};

	UPROPERTY(BlueprintReadWrite, Category = "PlayerCharacterAnimInstance|StateMachine", Meta = (DisplayName = "Vertical Alpha"))
	float VerticalAlpha {0.0f};

private:
	UPROPERTY(BlueprintReadOnly, Category = "PlayerCharacterAnimInstance", Meta = (Displayname = "Player Character", AllowPrivateAccess = "true", BlueprintProtected))
	APlayerCharacter* PlayerCharacter;

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
	UFUNCTION(BlueprintPure, Category = "PlayerCharacterAnimInstance", Meta = (DisplayName = "Get Footstep Data"))
	FFootstepData GetFootstepData(EFoot Foot);

private:
	void CheckMovementState(const APlayerCharacter& Character, const APlayerCharacterController& Controller, const UPlayerCharacterMovementComponent& CharacterMovement);
	
	void CheckTurnInplaceConditions(const APlayerCharacter& Character);

	static float GetDirection(const APlayerCharacter& Character);

	static float GetSpeed(const APlayerCharacter& Character, const UPlayerCharacterMovementComponent& CharacterMovement);

	void UpdateFallTime(const float& DeltaTime);
};
