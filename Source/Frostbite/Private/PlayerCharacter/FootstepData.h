// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "FootstepData.generated.h"


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
	UPROPERTY(BlueprintReadWrite, Category = FootstepData, Meta = (DisplayName = "Foot"))
	EFoot Foot;

	/** Location of the footstep in world space. */
	UPROPERTY(BlueprintReadWrite, Category = FootstepData, Meta = (DisplayName = "Location"))
	FVector Location;

	/** The PhysicalMaterial underneath the foot. */
	UPROPERTY(BlueprintReadWrite, Category = FootstepData, Meta = (DisplayName = "Physical Material"))
	class UPhysicalMaterial* PhysicalMaterial;
	
	/** The object underneath the foot. */
	UPROPERTY(BlueprintReadWrite, Category = FootstepData, Meta = (DisplayName = "Object Underneath Foot"))
	UObject* Object;
	
	/** Constructor with default values. */
	FFootstepData()
	{
		Foot = EFoot::Left;
		Location = FVector();
		PhysicalMaterial = nullptr;
		Object = nullptr;
	}

	/** Constructor. */
	FFootstepData(EFoot Foot, FVector Location, UPhysicalMaterial* PhysicalMaterial, UObject* Object);
};
