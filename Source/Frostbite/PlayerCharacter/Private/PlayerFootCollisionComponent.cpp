// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#include "PlayerFootCollisionComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"

#if WITH_REACOUSTIC
#include "Reacoustic/Public/ReacousticComponent.h"
#endif

UPlayerFootCollisionComponent::UPlayerFootCollisionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayerFootCollisionComponent::BeginPlay()
{
	Super::BeginPlay();

	Character = Cast<ACharacter>(GetOwner());
	
	CollisionSphere = Cast<USphereComponent>(GetOwner()->AddComponentByClass(USphereComponent::StaticClass(),
		false, this->GetRelativeTransform(), true));
	if (CollisionSphere)
	{
		CollisionSphere->SetupAttachment(this);
		CollisionSphere->InitSphereRadius(SphereRadius);
		
		CollisionSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
		CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		CollisionSphere->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
		CollisionSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap);

		CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &UPlayerFootCollisionComponent::OnOverlapBegin);

		CollisionSphere->RegisterComponent();
	}
}

void UPlayerFootCollisionComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	FVector ImpulseDirection = (OtherComp->GetComponentLocation() - GetComponentLocation()).GetSafeNormal();

	const FVector PlayerForwardVector {Character->GetActorForwardVector()};

	if (const float DotProduct {static_cast<float>(FVector::DotProduct(ImpulseDirection, PlayerForwardVector))}; DotProduct > 0.0f)
	{
		const FVector PlayerRightVector {Character->GetActorRightVector()};
		
		if (const float LateralDotProduct {static_cast<float>(FVector::DotProduct(ImpulseDirection, PlayerRightVector))}; LateralDotProduct < 0.0f)
			{
			ImpulseDirection -= PlayerRightVector * LateralOffsetFactor;
			}
		else 
			{
			ImpulseDirection += PlayerRightVector * LateralOffsetFactor;
			}
		ImpulseDirection.Normalize();
	}

	ImpulseDirection.Z = FMath::Max(0.0f, ImpulseDirection.Z);
	float ImpulseStrength;
	if (ScaleForceByMass)
	{
		const float ObjectMass {OtherComp->GetMass()};
		constexpr float MassExponent { 0.5f }; 
		const float ScaledMass = FMath::Pow(ObjectMass, MassExponent);
		ImpulseStrength = PushStrength * ScaledMass;
	}
	else
	{
		ImpulseStrength = PushStrength;
	}

	OtherComp->AddImpulse(ImpulseDirection * ImpulseStrength);

#if WITH_REACOUSTIC
if (ImpulseStrength > 1)
{
	if (UReacousticComponent* ReacousticComponent = Cast<UReacousticComponent>(OtherActor->GetComponentByClass(UReacousticComponent::StaticClass()));)
	{
		ReacousticComponent->TriggerManualHit(0.5f);
	}
}
#endif
};
