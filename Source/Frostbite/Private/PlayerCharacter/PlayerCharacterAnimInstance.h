// Copyright 2023 Barrelhouse

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "FootstepData.h"
#include "PlayerCharacterAnimInstance.generated.h"

class APlayerCharacter;

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
	UPROPERTY(BlueprintReadWrite, Category = "PlayerCharacterAnimInstance|StateMachine", Meta = (DisplayName = "Do Right Turn"))
	bool DoRightTurn {false};

private:
	UPROPERTY(BlueprintReadOnly, Category = "PlayerCharacterAnimInstance", Meta = (Displayname = "Player Character", AllowPrivateAccess = "true", BlueprintProtected))
	APlayerCharacter* PlayerCharacter;

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeBeginPlay() override;
	
	UFUNCTION(BlueprintPure, Category = "PlayerCharacterAnimInstance", Meta = (DisplayName = "Get Footstep Data"))
	FFootstepData GetFootstepData(EFoot Foot);

private:
	void CheckTurnInplaceConditions(const APlayerCharacter& Character);
};
