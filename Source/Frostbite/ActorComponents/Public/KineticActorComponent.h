// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KineticActorComponent.generated.h"

/** Component that is temporarily added to a static mesh actor when the player grabs it using the physics handler.
 *	Handles some logic related to physics and collision, and destroys itself after the physics object has successfully gone to sleep. */
UCLASS(NotBlueprintable, BlueprintType, ClassGroup = "Physics", Meta = (DisplayName = "Grab Handler Component",
	ShortToolTip = "Component that is added to an actor when the player grabs it."))
class UKineticActorComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	/** The minimum time the rigid body should stay awake after being released by the physics handler. */
	UPROPERTY(EditDefaultsOnly, Category = "Physics", Meta = (DisplayName = "Minimum Time To Stay Awake After Release"))
	float TimeToStayAwakeAfterRelease {3.0f};

	/** When set to true, Continious Collision Detection (CCD) will be enabled for the grabbed rigidbody untill it is put to sleep after being released. */
	UPROPERTY(EditDefaultsOnly, Category = "Collision", Meta = (DisplayName = "Enable Continious Collision Detection"))
	bool EnableCCD {true};

	/** Pointer to the mesh component of the actor that is simulating physics. */
	UPROPERTY()
	UStaticMeshComponent* Mesh;

	/** When true, the actor's bGenerateWakeEvents property should be set to false when this component wants to destroy itself. */
	UPROPERTY()
	bool DisableGenerateWakeEventsOnSleep {false};

	/** The time the component should wait before re-enabling NotifyRigidBodyCollision on the physics simulating scene component. */
	UPROPERTY()
	float CollisionHitEventEnableDelay {0.3f};

	/** The timer handle for re-enabling NotifyRigidBodyCollision on the physics simulating scene component. */
	UPROPERTY()
	FTimerHandle CollisionHitEventEnableTimerHandle;

public:	
	UKineticActorComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Handles the actor being grabbed. */
	UFUNCTION()
	void HandleOnOwnerGrabbed();

protected:
	virtual void BeginPlay() override;
	virtual void OnRegister() override;

private:
	/** Handles the termination of this component when the actor goes to sleep. */
	UFUNCTION()
	void HandleActorSleep(UPrimitiveComponent* Component, FName BoneName);
	
	/** Re-enables NotifyRigidBodyCollision on the physics simulating scene component. */
	UFUNCTION()
	void EnableNotifyRigidBodyCollisionOnOwner();
};
