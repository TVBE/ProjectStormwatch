// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHCollisionTriggerableComponent.h"

#include "Misc/UObjectToken.h"

#define LOCTEXT_NAMESPACE "BHCollisionTriggerableComponent"

DEFINE_LOG_CATEGORY_STATIC(LogBHCollisionTriggerableComponent, Log, All)

UBHCollisionTriggerableComponent::UBHCollisionTriggerableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBHCollisionTriggerableComponent::SetImpulseThreshold(float Value)
{
	ImpulseThreshold = Value;
}

void UBHCollisionTriggerableComponent::InitializeComponent()
{
	Super::InitializeComponent();

	UPrimitiveComponent* Target = GetTarget();
	if (!Target)
	{
		return;
	}
	Target->OnComponentHit.AddDynamic(this, &UBHCollisionTriggerableComponent::HandleOnComponentHit);
}

void UBHCollisionTriggerableComponent::UninitializeComponent()
{
	Super::UninitializeComponent();

	UPrimitiveComponent* Target = GetTarget();
	if (!Target)
	{
		return;
	}
	Target->OnComponentHit.RemoveDynamic(this, &UBHCollisionTriggerableComponent::HandleOnComponentHit);
}

void UBHCollisionTriggerableComponent::HandleOnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherComp->IsSimulatingPhysics())
	{
		return;
	}
	
	const FVector HitDirection = Hit.ImpactNormal.GetSafeNormal();
	const float Angle = static_cast<float>(FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(HitDirection, -GetComponentRotation().Vector()))));
	
	if (NormalImpulse.Size() >= ImpulseThreshold && (!bRestrictCollisionAngle || Angle <= MaxAllowedAngle))
	{
		OnCollisionTrigger.Broadcast(NormalImpulse);
	}
}

UPrimitiveComponent* UBHCollisionTriggerableComponent::GetTarget() const
{
	if (!IsValid(GetAttachParent()))
	{
		UE_LOG(LogBHCollisionTriggerableComponent, Warning,
			TEXT("GetTarget: Component [%s] does not have a valid attach parent."),
			*GetName());
		return nullptr;
	}

	UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(GetAttachParent());
	if (!PrimitiveComponent)
	{
		UE_LOG(LogBHCollisionTriggerableComponent, Warning,
			TEXT("GetTarget: Component [%s] is attached to a %s instead of a UPrimitiveComponent")
			TEXT("in Actor [%s]. UCollisionTriggerableComponents only work when attached to UPrimitiveComponents."),
			*GetName(), *GetAttachParent()->GetClass()->GetName(), *GetOwner()->GetName());
	}
	return PrimitiveComponent;
}

#if WITH_EDITOR
void UBHCollisionTriggerableComponent::CheckForErrors()
{
	Super::CheckForErrors();

	const AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}
	
	const UPrimitiveComponent* Target = GetTarget();
	if (!IsValid(Target))
	{
		return;
	}
	if (Target->GetGenerateOverlapEvents())
	{
		FFormatNamedArguments Arguments;
		Arguments.Add(TEXT("ComponentName"), FText::FromString(GetName()));
		Arguments.Add(TEXT("TargetName"), FText::FromString(Target->GetName()));
		FMessageLog("MapCheck").Warning()
			->AddToken(FUObjectToken::Create(Owner))
			->AddToken(FTextToken::Create(FText::Format( LOCTEXT( "MapCheck_Message_GenerateOverlapEvents",
				"{ComponentName}::{TargetName} has bGenerateOverlapEvents set to true. "
				"This needs to be disabled for collision triggers to work correctly." ), Arguments )));
	}
	if (const FBodyInstance* BodyInstance = Target->GetBodyInstance(); !BodyInstance->bNotifyRigidBodyCollision)
	{
		FFormatNamedArguments Arguments;
		Arguments.Add(TEXT("ComponentName"), FText::FromString(GetName()));
		Arguments.Add(TEXT("TargetName"), FText::FromString(Target->GetName()));
		FMessageLog("MapCheck").Warning()
			->AddToken(FUObjectToken::Create(Owner))
			->AddToken(FTextToken::Create(FText::Format( LOCTEXT( "MapCheck_Message_NotifyRigidBodyCollision",
				"{ComponentName}::{TargetName} has bNotifyRigidBodyCollision set to false. "
				"This needs to be enabled for collision triggers to work correctly." ), Arguments )));
	}
}
#endif

#undef LOCTEXT_NAMESPACE