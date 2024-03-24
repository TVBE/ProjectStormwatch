// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "Components/SceneComponent.h"
#include "BHCollisionTriggerableComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCollisionTriggerSignature, FVector, Impulse);

UCLASS(NotBlueprintable, BlueprintType, ClassGroup = "Barrelhouse", Meta = (BlueprintSpawnableComponent))
class UBHCollisionTriggerableComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UBHCollisionTriggerableComponent();
	
	UFUNCTION(BlueprintCallable)
	void SetImpulseThreshold(float Value);
	
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnCollisionTriggerSignature OnCollisionTrigger;

protected:
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	
	UPrimitiveComponent* GetTarget() const;

#if WITH_EDITOR
	virtual void CheckForErrors() override;
#endif

private:
	UFUNCTION()
	void HandleOnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit);

	// The impulse force threshold for the collision event to be triggered.
	UPROPERTY(EditAnywhere, Category = "Collision", Meta = (Displayname = "Force Threshold", Units = "Newtons"))
	float ImpulseThreshold = 100.0f;

	// If true, We expect the collision to come from a certain direction.
	UPROPERTY(Meta = (InlineEditConditionToggle))
	bool bRestrictCollisionAngle = false;
	
	// The maximum allowed angle for the collision to be triggered.
	UPROPERTY(EditAnywhere, Category = "Collision", Meta = (DisplayName = "Maximum Allowed Angle", Units = "Degrees",
		EditCondition = "RestrictCollisionAngle"))
	float MaxAllowedAngle = 45.0f;
};
