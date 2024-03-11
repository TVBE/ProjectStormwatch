// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "BHPlayerCharacterComponent.h"
#include "BHPlayerInteractionComponent.generated.h"

// START DEPRECATED -------------------------------------------------------------


UENUM(BlueprintType)
enum class EBHInteractionActionType : uint8
{
	Primary					UMETA(DisplayName = "Primary", ToolTip = "The primary action is used to 'use' actors."),
	Secondary				UMETA(DisplayName = "Secondary", ToolTip = "The secondary action is used to grab actors."),
	Inventory				UMETA(DisplayName = "Inventory", ToolTip = "The inventory action is used to add or take objects from the inventory.")
};

USTRUCT(BlueprintType)
struct FBHInteractableObjectData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly)
	UObject* Object = nullptr;

	UPROPERTY(BlueprintReadOnly)
	EBHInteractionType InteractionType = EBHInteractionType::Grabbable;

	FBHInteractableObjectData()
	{
	}

	FBHInteractableObjectData(UObject* InObject, EBHInteractionType InInteractionType)
		: Object(InObject)
		, InteractionType(InInteractionType)
	{
	}
};

UENUM(BlueprintType)
enum class EBHInteractionType : uint8
{
	Usable,
	Grabbable,
	Draggable,
	None
};

// END DEPRECATED -------------------------------------------------------------

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBeginInteraction, EBHInteractionType, Type, USceneComponent*, Component);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEndInteraction, EBHInteractionType, Type, USceneComponent*, Component);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNewInteractableObjectFoundSignature, UObject*, Object, EBHInteractionType, InteractionType);

UCLASS(NotBlueprintable, BlueprintType, ClassGroup = "Barrelhouse")
class STORMWATCH_API UBHPlayerInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBHPlayerInteractionComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	bool IsCharacterInteracting(const class ABHPlayerCharacter* Character) const;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnNewInteractableObjectFoundSignature OnNewInteractableObjectFound;

private:
	// Collects all interactable objects in a radius. Result is sorted by priority and distance.
	TArray<TPair<UObject*, FVector>> TraceForInteractableObjects(const FVector& Location,
		const TArray<AActor*>& IgnoredActors);

	bool IsOccluded(UObject* Object, const FVector& Start, const FVector& End);
	static EBHInteractionType DetermineInteractionType(const UObject* Object);
	static FVector GetNearestPointOnMesh(const FHitResult& HitResult, const AActor* Actor)
	
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
	
	UPROPERTY(EditsDefaultOnly, Category = "Interaction", Meta = (EditCondition = "bUsePriority", InlineEditConditionToggle))
	bool bUsePriorityDistanceThreshold = true;

	// Minimum distance for interaction priority consideration.
	// Prevents higher priority objects from overtaking lower priority objects when looked at directly.
	UPROPERTY(EditDefaultsOnly, Category = "Interaction",
	Meta = (EditCondition = "bUsePriority && bUsePriorityDistanceThreshold", Units = "Centimeters",
			ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100"))
	float PriorityDistanceThreshold = 10.0f;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, Category = "Debugging")
	bool bEnableDebugVisualisation = false;
#endif
	
	TOptional<TPair<TWeakObjectPtr<UObject>, EBHInteractionType>> CurrentInteractableObject;
};
