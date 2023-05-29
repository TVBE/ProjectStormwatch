// Copyright Notice

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NightstalkerAIFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class UNightstalkerAIFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Checks if a point is occluded from the perspective of another point.
	* @param WorldContextObject The context object required to get a reference to the world.
	* @param PointA The starting point for the occlusion check.
	* @param PointB The ending point for the occlusion check.
	* @param DrawDebugLines If true, draw debug lines representing the occlusion check.
	* @param DebugLineDuration The duration for which the debug lines should be drawn. */
	UFUNCTION(BlueprintPure, Category = "Nightstalker AI Helpers", Meta = (
		WorldContext = "WorldContextObject",
		DisplayName = "Is Occluded",
		Keywords = "Is Occluded Occlusion Visibility Visible",
		AdvancedDisplay = "DrawDebugLines, DebugLineDuration"))
	static bool IsOccluded(UObject* WorldContextObject, const FVector& PointA,
		const FVector& PointB, const bool DrawDebugLines = false, const float DebugLineDuration = 0.0f);
	
	/** Performs a fast check to see if a point is occluded from the perspective of the another point.
	* @param WorldContextObject The context object required to get a reference to the world.
	* @param LocationA The starting point for the occlusion check.
	* @param LocationB The ending point for the occlusion check.
	* @param DrawDebugLines If true, draw debug lines representing the occlusion check.
	* @param DebugLineDuration The duration for which the debug lines should be drawn.
	* @note This function is more performant than the regular 'IsActorOccluded' check, but is less accurate. */
	UFUNCTION(BlueprintPure, Category = "Nightstalker AI Helpers", Meta = (
		WorldContext = "WorldContextObject",
		DisplayName = "Is Occluded (Fast)",
		Keywords = "Is Occluded Occlusion Visibility Visible",
		AdvancedDisplay = "DrawDebugLines, DebugLineDuration"))
	static bool IsOccludedFast(UObject* WorldContextObject, const  FVector& LocationA,
		const FVector& LocationB, const bool DrawDebugLines, const float DebugLineDuration = 0.0f);

	/** Calculates the angle between a forward direction (rotation) and a target from a specified origin.
	* @param Rotation The forward rotation representing the viewing direction.
	* @param Origin The origin from which the viewing direction is evaluated.
	* @param Target The target location towards which the angle should be calculated.
	* @param IgnorePitch If true, the function ignores the Z components of the vectors when computing the angle.
	* @return The angle in degrees between the forward direction and the direction to the target.
	* @note If IgnorePitch is set to true, the function computes the angle based on the projections of the vectors onto the XY plane. */
	UFUNCTION(BlueprintPure, Category = "Nightstalker AI Helpers", Meta = (
		DisplayName = "Get Player View Angle"))
	static float GetViewAngle(const FRotator& Rotation, const FVector& Origin, const FVector& Target, bool IgnorePitch);
};
