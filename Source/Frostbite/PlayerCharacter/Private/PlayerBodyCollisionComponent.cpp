// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#include "PlayerBodyCollisionComponent.h"

UPlayerBodyCollisionComponent::UPlayerBodyCollisionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UPlayerBodyCollisionComponent::OnRegister()
{
	Super::OnRegister();

	SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SetCollisionResponseToAllChannels(ECR_Ignore);
	SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
}

void UPlayerBodyCollisionComponent::BeginPlay()
{
	Super::BeginPlay();
	OnComponentBeginOverlap.AddDynamic(this, &UPlayerBodyCollisionComponent::OnBeginOverlap);
	OnComponentEndOverlap.AddDynamic(this, &UPlayerBodyCollisionComponent::OnEndOverlap);
}

void UPlayerBodyCollisionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PushedComponents.IsEmpty()) { return; }

	TArray<UPrimitiveComponent*> ComponentsToRemove;
	for (UPrimitiveComponent* PushedComponent : PushedComponents)
	{
		if (PushedComponent && PushedComponent->IsSimulatingPhysics())
		{
			const float Mass {PushedComponent->GetMass()};

			FVector CenterOfMass {PushedComponent->GetCenterOfMass()};
			FVector Direction {(CenterOfMass - GetComponentLocation()).GetSafeNormal()};
			const FVector ImpulseToApply {Direction * PushForce * DeltaTime};

			if (Mass <= BounceThreshold)
			{
				const FVector BounceImpulseVector {FVector(ImpulseToApply.X, ImpulseToApply.Y, 0.0f) * BounceForce};
				PushedComponent->AddImpulse(BounceImpulseVector, NAME_None, true);
				ComponentsToRemove.Add(PushedComponent);
			}
			else
			{
				const FVector ImpulseToApplyXY {FVector(ImpulseToApply.X, ImpulseToApply.Y, 0.0f) * (Mass / 100.0f)};
				PushedComponent->AddImpulse(ImpulseToApplyXY, NAME_None, true);
			}
		}
	}

	for (UPrimitiveComponent* ComponentToRemove : ComponentsToRemove)
	{
		PushedComponents.Remove(ComponentToRemove);
	}
}

void UPlayerBodyCollisionComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherComp && OtherComp->IsSimulatingPhysics())
	{
		PushedComponents.AddUnique(OtherComp);

		if (PushedComponents.Num() > 0 && !IsComponentTickEnabled())
		{
			SetComponentTickEnabled(true);
		}
	}
}

void UPlayerBodyCollisionComponent::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	PushedComponents.Remove(OtherComp);

	if (PushedComponents.Num() == 0 && IsComponentTickEnabled())
	{
		SetComponentTickEnabled(false);
	}
}

float UPlayerBodyCollisionComponent::GetTotalMassOfPushedObjects() const
{
	float TotalMass = 0.0f;
	for (const UPrimitiveComponent* PushedComponent : PushedComponents)
	{
		if (PushedComponent)
		{
			TotalMass += PushedComponent->GetMass();
		}
	}

	return TotalMass;
}


