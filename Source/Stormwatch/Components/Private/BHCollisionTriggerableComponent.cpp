// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHCollisionTriggerableComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogBHCollisionTriggerableComponent, Log, All)

UBHCollisionTriggerableComponent::UBHCollisionTriggerableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBHCollisionTriggerableComponent::SetImpulseForceThreshold(float Value)
{
	ImpulseForceThreshold = Value;
}

void UBHCollisionTriggerableComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UStaticMeshComponent* Component = Cast<UStaticMeshComponent>(GetAttachParent()))
	{
		Component->OnComponentHit.AddDynamic(this, &UBHCollisionTriggerableComponent::OnStaticMeshComponentHit);
		Component->SetGenerateOverlapEvents(false);
		Component->SetNotifyRigidBodyCollision(true);

		MeshComponent = Component;
	}
}

void UBHCollisionTriggerableComponent::OnStaticMeshComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
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

UPrimitiveComponent* UBHCollisionTriggerableComponent::GetTarget() const
{
	if (IsValid(GetAttachParent()))
	{
		UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(GetAttachParent()
		if (!PrimitiveComponent)
		{
			UE_LOG(LogBHCollisionTriggerableComponent, Warning, TEXT("%s is attached to a %s instead of a UPrimitiveComponent")
			TEXT("in %s. UCollisionTriggerableComponents only work when attached to UPrimitiveComponents."),
			*GetName(), *GetAttachParent()->GetClass()->GetName(), *GetOwner()->GetName());
		}
		return PrimitiveComponent;
	}
	UE_LOG(LogBHCollisionTriggerableComponent, Warning, TEXT("%s is not attached to a valid component."), *GetName())
	return nullptr;
}






