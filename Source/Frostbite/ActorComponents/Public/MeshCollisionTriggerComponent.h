// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MeshCollisionTriggerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCollisionTriggerDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCollisionTriggerLimitReachedDelegate);

class UArrowComponent;

UCLASS(NotBlueprintable, BlueprintType, ClassGroup = "Physics", Meta = (BlueprintSpawnableComponent, Displayname = "Collision Trigger Component"))
class UMeshCollisionTriggerComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	/** Delegate that is called when a collision with sufficient force and at the correct angle has occured on the mesh. */
	UPROPERTY(BlueprintAssignable, Category = "Collision Trigger Component", Meta = (DisplayName = "On Collision Trigger"))
	FOnCollisionTriggerDelegate OnCollisionTrigger;

	/** Delegate that is called when the max collision trigger limit, if enabled, is reached. */
	UPROPERTY(BlueprintAssignable, Category = "Collision Trigger Component", Meta = (DisplayName = "On Collision Trigger Limit Reached."))
	FOnCollisionTriggerLimitReachedDelegate OnCollisionTriggerLimitReached;

private:
	UPROPERTY()
	UStaticMeshComponent* MeshComponent;
	
	/** The impulse force threshold for the collision event to be triggered. */
	UPROPERTY(EditAnywhere, Category = "Collision Trigger Settings")
	float ImpulseForceThreshold {1000.0f};

	/** If true, We expect the collision to come from a certain direction. */
	UPROPERTY(EditAnywhere, Category = "Collision Trigger Settings", Meta = (DisplayName = "Restrict Collision Angle", InlineEditConditionToggle))
	bool RestrictCollisionAngle {false};
	
	/** The maximum allowed angle for the collision to be triggered. */
	UPROPERTY(EditAnywhere, Category = "Collision Trigger Settings", Meta = (DisplayName = "Maximum Allowed Angle",
		EditCondition = "RestrictCollisionAngle"))
	float MaxAllowedAngle {45.0f};

	/** If true, we enforce a max trigger count for this collision trigger component.
	 *	If the limit is reached, OnCollisionTriggerLimitReached is broadcast.
	 *	It is possible to reset the trigger limit afterwards. */
	UPROPERTY(EditAnywhere, Category = "Collision Trigger Settings", Meta = (DisplayName = "Enable Trigger Limit"))
	bool IsTriggerLimitEnabled {false};

	UPROPERTY(EditAnywhere, Category = "Collision Trigger Settings", Meta = (DisplayName = "Trigger Limit",
		EditCondition = "IsTriggerLimitEnabled", EditConditionHides))
	int32 TriggerLimit {1};

	UPROPERTY()
	int32 Triggers {0};

	/** If true, the collision trigger component destroys itself after the trigger limit is reached. */
	UPROPERTY(EditAnywhere, Category = "Collision Trigger Settings", Meta = (DisplayName = "Auto Destroy",
		EditCondition = "IsTriggerLimitEnabled", EditConditionHides))
	bool DestroyAfterTriggerLimitReached {true};

public:	
	UMeshCollisionTriggerComponent();
	
	/** Resets the trigger count. */
	UFUNCTION(BlueprintCallable, Category = "Collision Trigger Component", Meta = (DisplayName = "Reset Trigger Count"))
	FORCEINLINE void ResetTriggerCount() { Triggers = 0; }

	/** Sets the force threshold required to cause a collision trigger. */
	UFUNCTION(BlueprintCallable, Category = "Collision Trigger Component", Meta = (DisplayName = "Set Force Threshold"))
	FORCEINLINE void SetImpulseForceThreshold(const float Value) { ImpulseForceThreshold = Value; }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION()
	void OnStaticMeshComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	


};
