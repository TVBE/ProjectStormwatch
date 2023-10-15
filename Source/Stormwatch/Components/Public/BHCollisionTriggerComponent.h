// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "BHCollisionTriggerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCollisionTriggerDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCollisionTriggerLimitReachedDelegate);

UCLASS(NotBlueprintable, BlueprintType, ClassGroup = "Physics", Meta = (BlueprintSpawnableComponent))
class UBHCollisionTriggerComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UBHCollisionTriggerComponent();
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void ResetTriggerCount();
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetImpulseForceThreshold(const float Value);
	
	UPROPERTY(BlueprintAssignable)
	FOnCollisionTriggerDelegate OnCollisionTrigger;
	
	UPROPERTY(BlueprintAssignable)
	FOnCollisionTriggerLimitReachedDelegate OnCollisionTriggerLimitReached;

private:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UFUNCTION()
	void OnStaticMeshComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit);
	
	UPROPERTY()
	UStaticMeshComponent* MeshComponent = nullptr;

	// The impulse force threshold for the collision event to be triggered.
	UPROPERTY(EditAnywhere, Category = "Collision", Meta = (Displayname = "Force Threshold", Units = "Newtons"))
	float ImpulseForceThreshold = 100.0f;

	// If true, We expect the collision to come from a certain direction.
	UPROPERTY(Category = "Collision", Meta = (InlineEditConditionToggle))
	bool bRestrictCollisionAngle = false;
	
	// The maximum allowed angle for the collision to be triggered.
	UPROPERTY(EditAnywhere, Category = "Collision", Meta = (DisplayName = "Maximum Allowed Angle", Units = "Degrees",
		EditCondition = "RestrictCollisionAngle"))
	float MaxAllowedAngle = 45.0f;

	// If true, we enforce a max trigger count for this collision trigger component.
	// If the limit is reached, OnCollisionTriggerLimitReached is broadcast.
	// It is possible to reset the trigger limit afterwards.
	UPROPERTY(EditAnywhere, Category = "Collision", Meta = (DisplayName = "Enable Trigger Limit"))
	bool bEnableTriggerLimit = false;
	
	UPROPERTY(EditAnywhere, Category = "Collision", Meta = (EditCondition = "IsTriggerLimitEnabled", EditConditionHides))
	int32 TriggerLimit = 1;

	UPROPERTY()
	int32 Triggers = 0;

	// If true, the collision trigger component destroys itself after the trigger limit is reached.
	UPROPERTY(EditAnywhere, Category = "Collision", Meta = (DisplayName = "Auto Destroy",
		EditCondition = "IsTriggerLimitEnabled", EditConditionHides))
	bool DestroyAfterTriggerLimitReached = true;
};
