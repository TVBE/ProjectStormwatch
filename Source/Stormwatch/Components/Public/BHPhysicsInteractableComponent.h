// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "BHInteractableComponent.h"
#include "BHPhysicsInteractableComponent.generated.h"

USTRUCT()
struct FBHCollisionSettingsSnapshot
{
	GENERATED_BODY()
	
	bool bEnableCDO;
	ESleepFamily SleepFamily;
	float SleepThresholdMultiplier;
	bool bGenerateWakeEvents;
	
	FBHCollisionSettingsSnapshot()
	: bEnableCDO(false),
	  SleepFamily(ESleepFamily::Normal),
	  SleepThresholdMultiplier(0.0f),
	  bGenerateWakeEvents(false)
	{}
	
	FBHCollisionSettingsSnapshot(
		bool InEnableCDO,
		ESleepFamily InOriginalSleepFamily,
		float InOriginalSleepThreshold,
		bool InDisableGenerateWakeEventsOnSleep
	)
		: bEnableCDO(InEnableCDO),
		  SleepFamily(InOriginalSleepFamily),
		  SleepThresholdMultiplier(InOriginalSleepThreshold),
		  bGenerateWakeEvents(InDisableGenerateWakeEventsOnSleep)
	{}

	FBHCollisionSettingsSnapshot(const UPrimitiveComponent* InPrimitiveComponent)
	{
		if(ensure(InPrimitiveComponent))
		{
			bEnableCDO = InPrimitiveComponent->BodyInstance.bUseCCD;
			SleepFamily = InPrimitiveComponent->BodyInstance.SleepFamily;
			SleepThresholdMultiplier = InPrimitiveComponent->BodyInstance.CustomSleepThresholdMultiplier;
			bGenerateWakeEvents = InPrimitiveComponent->BodyInstance.bGenerateWakeEvents;
		}
		else
		{
			// Default values if the provided pointer is null.
			bEnableCDO = false;
			SleepFamily = ESleepFamily::Normal;
			SleepThresholdMultiplier = 1.0f;
			bGenerateWakeEvents = false;
		}
	}
};

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "Barrelhouse")
class UBHPhysicsInteractableComponent : public UBHInteractableComponent
{
	GENERATED_BODY()

public:	
	UBHPhysicsInteractableComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void OnRegister() override;

	virtual void BeginInteraction() override;
	virtual void EndInteraction() override;
	
#if WITH_EDITOR
	virtual void CheckForErrors() override;
#endif
	
private:
	UFUNCTION()
	void HandleRigidBodySleep(UPrimitiveComponent* Component, FName BoneName);
	
	UFUNCTION()
	void EnableNotifyRigidBodyCollisionOnAttachParent();
	
	UFUNCTION()
	void EnableRigidBodySleep();
	
	UPrimitiveComponent* GetPrimitiveComponent() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Physics Interaction",
	Meta = (AllowPrivateAccess = "true"))
	bool bEnableCDOOnInteraction = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Physics Interaction",
	Meta = (Units = "Seconds", ClampMin = "0", UIMin = "0", AllowPrivateAccess = "true"))
	float TimeToStayAwakeAfterRelease = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Physics Interaction",
	Meta = (Units = "Seconds", ClampMin = "0", UIMin = "0", AllowPrivateAccess = "true"))
	float TimeToDisableCollisionNotifiesOnInteraction = 0.3f;
	
	TOptional<FBHCollisionSettingsSnapshot> Snapshot;
	
	UPROPERTY()
	FTimerHandle RigidBodySleepEnableTimerHandle;
	
	UPROPERTY()
	FTimerHandle CollisionHitEventEnableTimerHandle;
};
