// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "BHPlayerInteractionComponent.generated.h"

UENUM(BlueprintType)
enum class EBHInteractionType : uint8
{
	Draggable,
	Grabbable,
	Usable,
	None
};

USTRUCT(BlueprintType)
struct FBHInteractionObject
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly)
	UObject* Object = nullptr;

	UPROPERTY(BlueprintReadOnly)
	EBHInteractionType InteractionType = EBHInteractionType::None;

	FBHInteractionObject()
	{
	}

	FBHInteractionObject(UObject* InObject, EBHInteractionType InInteractionType)
		: Object(InObject)
		, InteractionType(InInteractionType)
	{
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNewInteractableObjectFoundSignature, UObject*, Object, EBHInteractionType, InteractionType);

UCLASS(NotBlueprintable, BlueprintType, ClassGroup = "Barrelhouse")
class STORMWATCH_API UBHPlayerInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBHPlayerInteractionComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintPure, Category = "BHPlayerInteractionComponent")
	bool IsCharacterInteracting() const;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnNewInteractableObjectFoundSignature OnNewInteractableObjectFound;

private:
	// Collects all interactable objects in a radius. Result is sorted by priority and distance.
	TArray<TPair<UObject*, FVector>> TraceForInteractableObjects(const FVector& Location,
		const TArray<AActor*>& IgnoredActors);

	bool IsOccluded(UObject* Object, const FVector& Start, const FVector& End);
	static EBHInteractionType DetermineInteractionType(const UObject* Object);
	static FVector GetNearestPointOnMesh(const FHitResult& HitResult, const AActor* Actor);
	
	// Maximum distance to check for interactable objects. Has no effect while interacting.
	UPROPERTY(EditDefaultsOnly, Category = "Interaction",
	Meta = (Units = "Centimeters", ClampMin = "0", ClampMax = "500", UIMin = "0", UIMax = "500"))
	float MaxInteractionDistance = 250.0f;

	// Radius for detecting interactable objects from the player's focus point.
	UPROPERTY(EditDefaultsOnly, Category = "Interaction",
	Meta = (Units = "Centimeters", ClampMin = "0", ClampMax = "500", UIMin = "0", UIMax = "500"))
	float InteractionTraceRadius = 50.0f;

	// Toggle interaction priority checking.
	// Disable to reduce overhead from 'slow' UInterface calls via Unreal's reflection system.
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	bool bUsePriority = true;
	
	UPROPERTY(EditDefaultsOnly, Category = "Interaction", Meta = (EditCondition = "bUsePriority", InlineEditConditionToggle))
	bool bUsePriorityDistanceThreshold = true;

	// Minimum distance for interaction priority consideration.
	// Prevents higher priority objects from overtaking lower priority objects when looked at directly.
	UPROPERTY(EditDefaultsOnly, Category = "Interaction",
	Meta = (EditCondition = "bUsePriority && bUsePriorityDistanceThreshold", Units = "Centimeters",
			ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100"))
	float PriorityDistanceThreshold = 10.0f;

	// Enable rotation scaling based on object weight.
	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Rotation")
	bool bScalePawnRotationByWeight = true;

	// Input range for weight-based rotation scaling (kg).
	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Rotation",
	Meta = (EditCondition = "bScalePawnRotationByWeight", ClampMin = "0", UIMin = "0"))
	FVector2f RotationScalarWeightInputRange {8.0f, 25.0f};

	// Output range for weight-based rotation scaling factor.
	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Rotation",
	Meta = (EditCondition = "bScalePawnRotationByWeight", ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	FVector2f RotationScalarWeightOutputRange = {1.0f, 0.4f};

	// Enable rotation scaling based on object size.
	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Rotation")
	bool bScalePawnRotationBySize = true;

	// Input range for size-based rotation scaling (m^3).
	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Rotation",
	Meta = (EditCondition = "bScalePawnRotationBySize", ClampMin = "0", UIMin = "0"))
	FVector2f RotationScalarSizeInputRange = {0.5f, 2.0f};

	// Output range for size-based rotation scaling factor.
	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Rotation",
	Meta = (DisplayName = "Size Scalars", ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	FVector2f RotationScalarSizeOutputRange = {1.0f, 0.4f};

	// Enable rotation scaling based on distance to object.
	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Rotation")
	bool bScalePawnRotationByDistance = true;

	// Input range for distance-based rotation scaling (cm).
	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Rotation",
	Meta = (DisplayName = "Distance Range", ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "200"))
	FVector2f RotationScalarDistanceInputRange {25.0f, 150.0f};

	// Output range for distance-based rotation scaling factor.
	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Rotation",
	Meta = (DisplayName = "Size Scalars", ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	FVector2f RotationScalarDistanceOutputRange {1.0f, 0.5f};

	// Enable rotation scaling based on view center offset.
	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Rotation")
	bool bScalePawnRotationByOffset = true;

	// Input range for offset-based rotation scaling (cm).
	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Rotation",
	Meta = (EditCondition = "bScalePawnRotationByOffset", ClampMin = "0", UIMin = "0"))
	FVector2f RotationScalarOffsetInputRange {8.0f, 20.0f};

	// Output range for offset-based rotation scaling factor.
	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Rotation",
	Meta = (EditCondition = "bScalePawnRotationByOffset", ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	FVector2f RotationScalarOffsetOutputRange {1.0f, 0.1f};

	// Minimum rotation scaling factor.
	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Rotation",
	Meta = (DisplayName = "Multiplier Floor", ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	float RotationScalarFloor = 0.4f;

	// Enable interaction speed scaling based on object weight.
	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Speed")
	bool bScalePawnSpeedByWeight = true;

	// Input range for weight-based interaction speed scaling (kg).
	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Speed",
	Meta = (EditCondition = "bScalePawnSpeedByWeight", ClampMin = "0", UIMin = "0"))
	FVector2f SpeedScalarWeightInputRange = {8, 25};

	// Output range for weight-based interaction speed scaling factor.
	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Speed",
	Meta = (EditCondition = "bScalePawnSpeedByWeight", ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	FVector2f SpeedScalarWeightOutputRange = {1.0, 0.6};

	// Enable interaction speed scaling based on object size.
	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Speed")
	bool bScalePawnSpeedBySize = true;

	// Input range for size-based interaction speed scaling (m^3).
	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Speed",
	Meta = (EditCondition = "bScalePawnSpeedBySize", ClampMin = "0", UIMin = "0"))
	FVector2f SpeedScalarSizeInputRange = {0.5, 1.0};

	// Output range for size-based interaction speed scaling factor.
	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Speed",
	Meta = (EditCondition = "bScalePawnSpeedBySize", ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	FVector2f SpeedScalarSizeOutputRange = {1.0f, 0.6f};

	// Minimum interaction speed scaling factor.
	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Speed",
	Meta = (ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	float SpeedScalarFloor = 0.6f;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, Category = "Debugging")
	bool bEnableDebugVisualisation = false;
#endif
	
	TOptional<FBHInteractionObject> CurrentInteractableObject;
};
