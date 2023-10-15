// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHCollisionTriggerComponent.h"

UBHCollisionTriggerComponent::UBHCollisionTriggerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBHCollisionTriggerComponent::ResetTriggerCount()
{
	Triggers = 0;
}

void UBHCollisionTriggerComponent::SetImpulseForceThreshold(const float Value)
{
	ImpulseForceThreshold = Value;
}

void UBHCollisionTriggerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UStaticMeshComponent* Component = Cast<UStaticMeshComponent>(GetAttachParent()))
	{
		Component->OnComponentHit.AddDynamic(this, &UBHCollisionTriggerComponent::OnStaticMeshComponentHit);
		Component->SetGenerateOverlapEvents(false);
		Component->SetNotifyRigidBodyCollision(true);

		MeshComponent = Component;
	}
}

void UBHCollisionTriggerComponent::OnStaticMeshComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherComp->IsSimulatingPhysics())
	{
		return;
	}

	const FVector HitDirection = Hit.ImpactNormal.GetSafeNormal();
	const float Angle = static_cast<float>(FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(HitDirection, -GetComponentRotation().Vector()))));

	const FVector OtherVelocity = OtherComp->GetComponentVelocity();
	const FVector RelativeVelocity = OtherVelocity - MeshComponent->GetComponentVelocity();

	if (NormalImpulse.Size() >= ImpulseForceThreshold && (!bRestrictCollisionAngle || Angle <= MaxAllowedAngle))
	{
		OnCollisionTrigger.Broadcast();
		if (bEnableTriggerLimit)
		{
			Triggers++;
			if (Triggers >= TriggerLimit)
			{
				OnCollisionTriggerLimitReached.Broadcast();
				if (DestroyAfterTriggerLimitReached)
				{
					DestroyComponent();
				}
			}
		}
	}
}

void UBHCollisionTriggerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (UStaticMeshComponent* Component = Cast<UStaticMeshComponent>(GetAttachParent()))
	{
		Component->OnComponentHit.RemoveDynamic(this, &UBHCollisionTriggerComponent::OnStaticMeshComponentHit);
		
		MeshComponent = Component;
	}
}




