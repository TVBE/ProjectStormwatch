// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHDamageComponent.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"

UBHDamageComponent::UBHDamageComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBHDamageComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UStaticMeshComponent* Component = Cast<UStaticMeshComponent>(GetAttachParent()))
	{
		Component->OnComponentHit.AddDynamic(this, &UBHDamageComponent::OnStaticMeshComponentHit);

		if (const FBodyInstance* BodyInstance = Component->GetBodyInstance())
		{
			if (BodyInstance->bNotifyRigidBodyCollision)
			{
				Component->SetNotifyRigidBodyCollision(true);
			}
		}
		
		MeshComponent = Component;
	}
}

void UBHDamageComponent::ResetDamage()
{
	DamageLevel = 0.0f;
}

void UBHDamageComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (UStaticMeshComponent* Component = Cast<UStaticMeshComponent>(GetAttachParent()))
	{
		Component->OnComponentHit.RemoveDynamic(this, &UBHDamageComponent::OnStaticMeshComponentHit);
		
		MeshComponent = Component;
	}
}

void UBHDamageComponent::OnStaticMeshComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (Cast<ACharacter>(OtherActor))
	{
		return;
	}
	
	if ((!OtherComp->IsSimulatingPhysics() && !MeshComponent->IsSimulatingPhysics()) || IsCooldownActive)
	{
		return;
	}
	
	const float ImpulseForce = static_cast<float>(NormalImpulse.Size());
	
	if (ImpulseForce >= ImpulseForceThreshold)
	{
		AddDamage(ImpulseForce);
		StartCooldown();
	}
}

void UBHDamageComponent::AddDamage(const float Value)
{
	if (DamageLevel + Value >= Threshold)
	{
		DamageLevel = Threshold;
	}
	else
	{
		DamageLevel += Value;
	}
	UpdateDamagePercentage();
}

void UBHDamageComponent::SetDamageLevel(const float Value)
{
	DamageLevel = FMath::Clamp(Value, 0.0f, Threshold);
	UpdateDamagePercentage();
}

void UBHDamageComponent::UpdateDamagePercentage()
{
	const float OldDamagePercentage = DamagePercentage;
	DamagePercentage = (DamageLevel / Threshold) * 100.0f;
	
	if (DamagePercentage != OldDamagePercentage)
	{
		OnDamaged.Broadcast(DamagePercentage);
	}

	if (DamagePercentage == 100)
	{
		if (!IsThresholdReached)
		{
			OnDamageThresholdReached.Broadcast();
			HandleDamageThresholdReached();
		}
		IsThresholdReached = true;
	}
}

void UBHDamageComponent::HandleDamageThresholdReached()
{
	if (AActor* Owner = GetOwner())
	{
		if (PlayAudioEffects)
		{
			UGameplayStatics::SpawnSoundAtLocation(this, DestructionSound, GetComponentLocation());
		}
		
		if (PlayParticleEffects)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), DestructionNiagaraSystem, GetComponentLocation(), GetComponentRotation());
		}
		
		if (DestroyOwnerOnThresholdReached)
		{
			Owner->Destroy();
		}
	}
}

void UBHDamageComponent::StartCooldown()
{
	IsCooldownActive = true;
	
	if (const UWorld* World = GetWorld())
	{
		if (World->GetTimerManager().TimerExists(CooldownTimerHandle))
		{
			World->GetTimerManager().ClearTimer(CooldownTimerHandle);
			
		}

		constexpr float CooldownTime = 0.5;
		
		World->GetTimerManager().SetTimer(CooldownTimerHandle, this, &UBHDamageComponent::HandleCooldownFinished, CooldownTime, false);
	}
}

void UBHDamageComponent::HandleCooldownFinished()
{
	IsCooldownActive = false;
}



