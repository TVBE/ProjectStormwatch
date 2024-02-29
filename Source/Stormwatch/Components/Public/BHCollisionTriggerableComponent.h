// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "BHCollisionTriggerableComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCollisionTriggerDelegate);

UCLASS(NotBlueprintable, BlueprintType, ClassGroup = "Barrelhouse", Meta = (BlueprintSpawnableComponent))
class UBHCollisionTriggerableComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UBHCollisionTriggerableComponent();
	
	UFUNCTION(BlueprintCallable)
	void SetImpulseForceThreshold(float Value);
	
	UPROPERTY(BlueprintAssignable)
	FOnCollisionTriggerDelegate OnCollisionTrigger;

protected:
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	
	UPrimitiveComponent* GetTarget() const;

private:
	UFUNCTION()
	void OnStaticMeshComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit);

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
};
