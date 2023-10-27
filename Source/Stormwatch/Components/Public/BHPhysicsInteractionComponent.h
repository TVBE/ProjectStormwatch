// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "BHInteractionComponent.h"
#include "BHPhysicsInteractionComponent.generated.h"

UCLASS(NotBlueprintable, BlueprintType, ClassGroup = "Physics")
class UBHPhysicsInteractionComponent : public UBHInteractionComponent
{
	GENERATED_BODY()

public:	
	UBHPhysicsInteractionComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION()
	void HandleOnOwnerGrabbed();
	
	UFUNCTION()
	void HandleOnOwnerReleased();

private:
	virtual void BeginPlay() override;
	virtual void OnRegister() override;

	UFUNCTION()
	void HandleActorSleep(UPrimitiveComponent* Component, FName BoneName);
	
	UFUNCTION()
	void EnableNotifyRigidBodyCollisionOnOwner();
	
	UFUNCTION()
	void EnableRigidBodySleep();
	
	float TimeToStayAwakeAfterRelease = 3.0f;
	
	bool bEnableCCD = true;
	
	UPROPERTY()
	UStaticMeshComponent* Mesh = nullptr;
	
	ESleepFamily OriginalSleepFamily = ESleepFamily::Normal;
	
	float OriginalSleepThreshold = 0.0f;
	
	bool bDisableGenerateWakeEventsOnSleep = false;

	// The time the component should wait before re-enabling bNotifyRigidBodyCollision on
	// the physics simulating scene component.
	float CollisionHitEventEnableDelay = 0.3f;
	
	bool bIsGrabbed = false;
	
	UPROPERTY()
	FTimerHandle RigidBodySleepEnableTimerHandle;
	
	UPROPERTY()
	FTimerHandle CollisionHitEventEnableTimerHandle;
};
