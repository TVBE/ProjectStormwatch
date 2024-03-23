// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BHCharacter.generated.h"

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "Barrelhouse")
class STORMWATCH_API ABHCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABHCharacter();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure, Category = "BHCharacter")
	float GetClearanceAbovePawn(float TraceLength = 500.0f) const;

	UFUNCTION(BlueprintPure, Category = "BHCharacter")
	float GetSpeed() const;
	
private:
	UPROPERTY()
	float Speed = 0.0f;
};
