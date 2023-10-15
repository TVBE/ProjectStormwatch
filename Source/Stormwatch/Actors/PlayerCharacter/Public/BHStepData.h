// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BHStepData.generated.h"

class UPhysicalMaterial;

UENUM(BlueprintType)
enum class EBHLeftRight : uint8
{
	Left		UMETA(DisplayName = "Left"),
	Right		UMETA(DisplayName = "Right"),
};

USTRUCT(BlueprintType)
struct STORMWATCH_API FBHStepData
{
	GENERATED_USTRUCT_BODY()
	
	/** Location of the step in world space. */
	UPROPERTY(BlueprintReadWrite, Category = "StepData", Meta = (DisplayName = "Location"))
	FVector Location;

	/** The velocity of the character when the step was made. */
	UPROPERTY(BlueprintReadWrite, Category = "StepData", Meta = (DisplayName = "Velocity"))
	float Velocity;

	/** The PhysicalMaterial underneath the foot. */
	UPROPERTY(BlueprintReadWrite, Category = "StepData", Meta = (DisplayName = "Physical Material"))
	UPhysicalMaterial* PhysicalMaterial;
	
	/** The object underneath the foot. */
	UPROPERTY(BlueprintReadWrite, Category = "StepData", Meta = (DisplayName = "Object Underneath Foot"))
	UObject* Object;
	
	/** Constructor with default values. */
	FBHStepData()
	{
		Location = FVector();
		Velocity = 0.0f;
		PhysicalMaterial = nullptr;
		Object = nullptr;
	}

	/** Constructor. */
	FBHStepData(EBHLeftRight Foot, FVector Location, float Velocity, UPhysicalMaterial* PhysicalMaterial, UObject* Object);
};
