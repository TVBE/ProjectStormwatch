// Copyright 2023 Nino Saglia & Tim Verberne

#include "ReacousticComponent.h"
#include "ReacousticSubsystem.h"

#define LOG_CATEGORY(LogReacousticComponent);

// Sets default values for this component's properties
UReacousticComponent::UReacousticComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
	bAutoActivate = true;
	
}
// Called after the component has been fully created and registered, but before it has been initialized and is ready for use.
void UReacousticComponent::OnComponentCreated()
{
	Super::OnComponentCreated();
}

// Called when the game starts
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
	
	// Subscribe to the OnComponentHit delegate of the target StaticMeshComponent.
	if(MeshComponent)
	{
		MeshComponent->OnComponentHit.AddDynamic(this, &UReacousticComponent::HandleOnComponentHit);
	}
	else
	{
		UE_LOG(LogReacousticComponent, Warning, TEXT("Failed to get mesh component"));
	}
}

// Called when the game ends or the object is marked for destruction.
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

void UReacousticComponent::HandleOnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if(Hit.ImpactNormal.Length() > 0.001)
	{
		DeltaLocationDistance = FVector::Distance(LatestLocation, Hit.Location);
		DeltaHitTime =  FMath::Clamp((FPlatformTime::Seconds() - LatestTime), 0.0, 1.0);
		LatestLocation = Hit.Location;
		LatestTime = FPlatformTime::Seconds();
		
		if(DeltaLocationDistance > 0.0001 && DeltaHitTime > 0.0001)
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


// Called every frame
void UReacousticComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

