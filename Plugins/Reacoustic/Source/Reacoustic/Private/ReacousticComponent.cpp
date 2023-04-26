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
						// StaticMeshComponent->SetGenerateOverlapEvents(true);
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
	/** We perform a lot of filtering to prevent hitsounds from playing in unwanted situations.*/
	if(Hit.ImpactNormal.Length() > 0.0001)
	{
		DeltaLocationDistance = FVector::Distance(LatestLocation, Hit.Location);
		
		
		LatestLocation = Hit.ImpactPoint;
		
		
		
		/** prevent situations were sounds are hittng in the same location.*/
		if(DeltaLocationDistance > 0.01)
		{
			/** prevent situations were sounds are hittng in a short timespan.*/
			DeltaHitTime = FMath::Clamp((FPlatformTime::Seconds() - LatestTime), 0.0, 1.0);
			LatestTime = FPlatformTime::Seconds();
			if(DeltaHitTime > 0.001)
			{
				/** prevent situations were sounds are hittng in the same orientation.*/
				DeltaForwardVector =FVector::Distance(LatestForwardVector,this->GetOwner()->GetActorForwardVector());
				LatestForwardVector = this->GetOwner()->GetActorForwardVector();

				/** If there's a considerable time between hits, remove the hit history.*/
				if(DeltaHitTime > 0.3)
				{
					DeltaStateArray.Empty();
				}
				if(DeltaForwardVector >0.0001)
				{
					/** Prevent more erratic hits happening for a long time in the same location. Eg: An object glitching behind the wall.*/

					DeltaStateArray.Add(DeltaLocationDistance * DeltaHitTime + DeltaForwardVector);
					UE_LOG(LogReacousticComponent, Verbose, TEXT("DeltaStateArray Array Sum: %f"),(GetArraySum(DeltaStateArray)));
					if(GetArraySum(DeltaStateArray) > 10.0f || DeltaStateArray.Num() <= 5)
					{
							// Calculate the impact force
							ImpactForce = NormalImpulse.Size() / HitComp->GetMass();
							// Trigger the OnComponentHit event
							OnComponentHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
					}
					else{UE_LOG(LogReacousticComponent,Verbose,TEXT("Prevented hit by: STATE ARRAY"))}
				}
				else{UE_LOG(LogReacousticComponent,Verbose,TEXT("Prevented hit by: DELTA FORWARD VECTOR"))}
			}
			else{UE_LOG(LogReacousticComponent,Verbose,TEXT("Prevented hit by: DELTA HIT TIME"))}
		}
		else{UE_LOG(LogReacousticComponent,Verbose,TEXT("Prevented hit by: LOCATION DISTANCE"))}
		/** Remove the oldest delta location distance from the array if it exceeds the limit. */
		if (DeltaStateArray.Num() > 5)
		{
			DeltaStateArray.RemoveAt(0);
		}
	}
}




/** This implementation will likely allways be ovewrriden by a blueprint or function that needs to trigger a custom hit.*/
void UReacousticComponent::TriggerManualHit_Implementation(float Strength){}


void UReacousticComponent::OnComponentHit_Implementation(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
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


