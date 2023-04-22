// Copyright 2023 Nino Saglia & Tim Verberne

#include "ReacousticComponent.h"

#include "FileCache.h"
#include "ReacousticSubsystem.h"

#define LOG_CATEGORY(LogReacousticComponent);


UReacousticComponent::UReacousticComponent()
{

	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
	bAutoActivate = true;
	
}

void UReacousticComponent::OnComponentCreated()
{
	Super::OnComponentCreated();
}


void UReacousticComponent::BeginPlay()
{
	Super::BeginPlay();
	if(const UWorld* World {GetWorld()})
	{
		if(UReacousticSubsystem* Subsystem {World->GetSubsystem<UReacousticSubsystem>()})
		{
			Subsystem->RegisterComponent(this);
		}
	}
	else
	{
		const FString ComponentName {this->GetName()};
		UE_LOG(LogReacousticComponent, Warning, TEXT("%s was unable to register itself to the Reacoustic subsystem."), *ComponentName);
	}

	TArray<UActorComponent*> Components;
		if(GetOwner())
		{
			GetOwner()->GetComponents(Components);
			if(!Components.IsEmpty())
			{
				for (UActorComponent* Component : Components)
				{
					UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(Component); 
					if (StaticMeshComponent->IsSimulatingPhysics() && StaticMeshComponent->BodyInstance.bNotifyRigidBodyCollision)
					{
						MeshComponent = StaticMeshComponent;
						StaticMeshComponent->SetGenerateOverlapEvents(true);
						break;
					}
				}
			}
			if(!MeshComponent)
			{
				const FString ComponentName {this->GetName()};
				UE_LOG(LogReacousticComponent, Warning, TEXT("%s was unable to find a static mesh component with physics simulation enabled in its owner. "), *ComponentName);
			}
		}
	

	if(MeshComponent)
	{
		MeshComponent->OnComponentHit.AddDynamic(this, &UReacousticComponent::HandleOnComponentHit);
	}
	else
	{
		UE_LOG(LogReacousticComponent, Warning, TEXT("Failed to get mesh component"));
	}
}


void UReacousticComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if(const UWorld* World {GetWorld()})
	{
		if(UReacousticSubsystem* Subsystem {World->GetSubsystem<UReacousticSubsystem>()})
		{
			Subsystem->UnregisterComponent(this);
		}
	}
	Super::EndPlay(EndPlayReason);
}

float UReacousticComponent::GetScaledImpactValue(const FVector& NormalImpulse, const UPrimitiveComponent* HitComponent,
	const AActor* OtherActor)
{
	if (!HitComponent || !HitComponent->IsSimulatingPhysics() || !OtherActor) {return 0.0f; }

	const FVector RelativeVelocity {HitComponent->GetComponentVelocity() - OtherActor->GetVelocity()};
	const FVector ScaledImpulse {(NormalImpulse + RelativeVelocity) / HitComponent->GetMass()};
	return ScaledImpulse.Length();
}

FReacousticSoundData UReacousticComponent::GetSurfaceHitSoundX(const AActor* Actor, const UPhysicalMaterial* PhysicalMaterial)
{
	return FReacousticSoundData();
}

void UReacousticComponent::HandleOnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if(Hit.ImpactNormal.SquaredLength() > 0.00001)
	{
		DeltaLocationDistance = FVector::Distance(LatestLocation, Hit.Location);
		DeltaHitTime = FMath::Clamp((FPlatformTime::Seconds() - LatestTime), 0.0, 1.0);
		const double DeltaForwardVector{FVector::Distance(LatestForwardVector,this->GetOwner()->GetActorForwardVector())};
		LatestLocation = Hit.ImpactPoint;
		LatestTime = FPlatformTime::Seconds();
		LatestForwardVector = this->GetOwner()->GetActorForwardVector();
		
		if(DeltaLocationDistance > 0.001 && DeltaHitTime > 0.01 && DeltaForwardVector >0.05)
		{
			OnComponentHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
		}
	}
	


}

void UReacousticComponent::OnComponentHit_Implementation(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
}

float UReacousticComponent::ReturnImpactForce(UPrimitiveComponent* HitComp, AActor* OtherActor,UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	/** if the relative speed is fast enough, return the impact force to be used to determine the hit sound timestamp*/

	//FVector RelativeSpeedVector =
	
	//if (RelativeSpeedVector > MinimumSpeed)
	//{
	//	FVector ImpactForceVector =  ((HitComp->GetComponentVelocity() - OtherActor->GetVelocity()+NormalImpulse) / HitComp->GetMass());
	//	float ImpactForce =  ImpactForceVector::Size;
	//}
	//else
	//{
	//	ImpactForce = -1;
	//}
return 0;
}

double UReacousticComponent::ReturnDeltaTime()
{
	return DeltaHitTime;
}

double UReacousticComponent::ReturnDeltaLocationDistance()
{
	return DeltaLocationDistance;
}

void UReacousticComponent::TransferData(UReacousticSoundDataAsset* SoundDataArray, UReacousticSoundDataRef_Map* ReferenceMap, FReacousticSoundData MeshSoundDataIn)
{
	if(SoundDataArray && ReferenceMap)
	{
		ReacousticSoundDataAsset = SoundDataArray;
		UReacousticSoundDataRefMap = ReferenceMap;
		MeshAudioData = MeshSoundDataIn;
	}
}


