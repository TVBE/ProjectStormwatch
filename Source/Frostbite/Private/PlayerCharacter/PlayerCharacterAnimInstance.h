// Copyright 2023 Barrelhouse

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "FootstepData.h"
#include "PlayerCharacterAnimInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFootstepDelegate, FFootstepData, FootstepData);


UCLASS()
class UPlayerCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FFootstepDelegate OnFootstep;

protected:
	UFUNCTION(BlueprintPure, Category = Default, Meta = (DisplayName = "Get Footstep Data"))
	FFootstepData GetFootstepData(EFoot Foot);
};
