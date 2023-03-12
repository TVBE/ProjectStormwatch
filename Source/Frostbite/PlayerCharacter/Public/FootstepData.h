// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "FootstepData.generated.h"

class UPhysicalMaterial;

/** Enum for defining which foot performed a footstep. */
UENUM(BlueprintType)
enum class EFoot : uint8
{
	Left		UMETA(DisplayName = "Left"),
	Right		UMETA(DisplayName = "Right"),
};

/** Struct containing data about a footstep. */
USTRUCT(BlueprintType)
struct FFootstepData
{
	GENERATED_USTRUCT_BODY()

	/** Which foot performed the footstep. */
	UPROPERTY(BlueprintReadWrite, Category = "FootstepData", Meta = (DisplayName = "Foot"))
	EFoot Foot;

	/** Location of the footstep in world space. */
	UPROPERTY(BlueprintReadWrite, Category = "FootstepData", Meta = (DisplayName = "Location"))
	FVector Location;

	/** The velocity of the character when the footstep was made. */
	UPROPERTY(BlueprintReadWrite, Category = "FootstepData", Meta = (DisplayName = "Velocity"))
	float Velocity;

	/** The PhysicalMaterial underneath the foot. */
	UPROPERTY(BlueprintReadWrite, Category = "FootstepData", Meta = (DisplayName = "Physical Material"))
	UPhysicalMaterial* PhysicalMaterial;
	
	/** The object underneath the foot. */
	UPROPERTY(BlueprintReadWrite, Category = "FootstepData", Meta = (DisplayName = "Object Underneath Foot"))
	UObject* Object;
	
	/** Constructor with default values. */
	FFootstepData()
	{
		Foot = EFoot::Left;
		Location = FVector();
		Velocity = 0.0f;
		PhysicalMaterial = nullptr;
		Object = nullptr;
	}

	/** Constructor. */
	FFootstepData(EFoot Foot, FVector Location, float Velocity, UPhysicalMaterial* PhysicalMaterial, UObject* Object);
};
