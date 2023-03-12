// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

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
	/** The delegate to be broadcasted when the mesh encounters a footstep AnimNotify. */
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
	/** Pointer to the player character that owns the skeletal mesh component that this anim instance is driving. */
	UPROPERTY(BlueprintReadOnly, Category = "PlayerCharacterAnimInstance", Meta = (Displayname = "Player Character", AllowPrivateAccess = "true", BlueprintProtected))
	APlayerCharacter* PlayerCharacter;

protected:
	/** Is called after the AnimInstance object is created and all of its properties have been initialized, but before the animation update loop begins. */
	virtual void NativeInitializeAnimation() override;

	/** Is called when the animation update loop begins. */
	virtual void NativeBeginPlay() override;

	/** Is called every frame. */
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	/** Returns data about a footstep at the specified foot, like the object or physical material underneath the foot at the time of the footstep.
	 *	@Foot The foot that is performing the footstep.
	 *	@Return FootstepData structure containing relevant information about the location and velocity of the foot at the time of the footstep. 
	 */
	UFUNCTION(BlueprintPure, Category = "PlayerCharacterAnimInstance", Meta = (DisplayName = "Get Footstep Data"))
	FFootstepData GetFootstepData(EFoot Foot);

private:
	/** Checks the movement state of the character and updates certain state machine conditions. */
	void CheckMovementState(const APlayerCharacter& Character, const APlayerCharacterController& Controller, const UPlayerCharacterMovementComponent& CharacterMovement);

	/** Checks whether the character is turning in place, and updates certain state machine conditions accordingly. */
	void CheckTurnInplaceConditions(const APlayerCharacter& Character);

	/** Returns the direction the character is moving in. */
	static float GetDirection(const APlayerCharacter& Character);

	/** Returns the speed that the character is moving at. */
	static float GetSpeed(const APlayerCharacter& Character, const UPlayerCharacterMovementComponent& CharacterMovement);

	/** Updates the time the player is falling, if the player is falling. */
	void UpdateFallTime(const float DeltaTime);
};
