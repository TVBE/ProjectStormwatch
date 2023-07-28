// Copyright (c) 2022-present Nino Saglia. All Rights Reserved.
// Written by Nino Saglia.
#include "ReacousticComponent.h"
#include "ReacousticSoundAsset.h"

DEFINE_LOG_CATEGORY_CLASS(UReacousticComponent, LogReacousticComponent);

UReacousticComponent::UReacousticComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UReacousticComponent::BeginPlay()
{
	Super::BeginPlay();
	if(const UWorld* World {GetWorld()})
	{
		if(UReacousticSubsystem* Subsystem {World->GetSubsystem<UReacousticSubsystem>()})
		{
			Subsystem->RegisterComponent(this);
			CachedSubsystem = Subsystem;
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
					if (UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(Component))
					{
						if (StaticMeshComponent->IsSimulatingPhysics() && StaticMeshComponent->BodyInstance.bNotifyRigidBodyCollision)
						{
							MeshComponent = StaticMeshComponent;
							// StaticMeshComponent->SetGenerateOverlapEvents(true);
							break;
						}
					}
				}
				
			}
			if(!MeshComponent)
			{
				const FString OwnerName {this->GetOwner()->GetName()};
				UE_LOG(LogReacousticComponent, Warning, TEXT("%s was unable to find a static mesh component with physics simulation enabled in its owner. "), *OwnerName);
			}
			Initialize();
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

inline void UReacousticComponent::Initialize_Implementation(USoundBase* SoundBase /* = nullptr */)
{
	const UWorld* World = GetWorld();
	if (!World) 
	{
		return;
	}

	/** Make sure the subsystem is still referenced.*/
	CachedSubsystem = CachedSubsystem ? CachedSubsystem : GetWorld()->GetSubsystem<UReacousticSubsystem>();
	
	/** If ReacousticSoundAsset is null, get the asset from the subsystem, otherwise use the existing asset. */
	UReacousticSoundAsset* AssetToTransfer = ReacousticSoundAsset ? ReacousticSoundAsset : CachedSubsystem->GetMeshSoundAsset(MeshComponent);
	TransferData(AssetToTransfer, CachedSubsystem->ReacousticSoundAssociationMap);




	/** Make sure htere's an audio component.*/
	if(!AudioComponent)
	{
		AudioComponent = NewObject<UAudioComponent>(GetOwner());
		AudioComponent->RegisterComponent();
		AudioComponent->SetSound(SoundBase);
	}

	if(!ReacousticSoundAsset)
	{
		UE_LOG(LogReacousticComponent, Warning, TEXT("No sound asset found, destroying component"));
		AudioComponent->Stop();
		DestroyComponent();
	}
	
		/** Set the attenuation settings */
		if(ReacousticSoundAsset->Sound_Attenuation){AudioComponent->AttenuationSettings = ReacousticSoundAsset->Sound_Attenuation;}

		if(ReacousticSoundAsset->Sound){AudioComponent->SetObjectParameter(TEXT("Obj_WaveAsset"), ReacousticSoundAsset->Sound);}


		/** Set the parameters.*/
		if(ReacousticSoundAsset->ImpulseLength){AudioComponent->SetFloatParameter(TEXT("Obj_Length"), ReacousticSoundAsset->ImpulseLength);}
	
	
	/** Add the audio component to the parent actor */
	AudioComponent->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
}

float UReacousticComponent::CalculateImpactValue(const FVector& NormalImpulse, const UPrimitiveComponent* HitComponent,
	const AActor* OtherActor)
{
	if (!HitComponent || !HitComponent->IsSimulatingPhysics() || !OtherActor) {return 0.0f; }

	const FVector RelativeVelocity {HitComponent->GetComponentVelocity() - OtherActor->GetVelocity()};
	//Rotation is currently not used since it results in unpredicable sound.
	const float RotationalSpeed = FMath::Abs(HitComponent->GetMass()*HitComponent->GetPhysicsAngularVelocityInRadians().Length());
	const FVector ScaledImpulse {(NormalImpulse + RelativeVelocity)};
	const float D = FMath::Abs(FVector::DotProduct(RelativeVelocity.GetSafeNormal(), NormalImpulse.GetSafeNormal()));
	return  RelativeVelocity.Length()*D;
}



void UReacousticComponent::HandleOnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if(FilterImpact(HitComp,OtherActor,OtherComp,NormalImpulse,Hit))
	{
		/** Make sure the subsystem is still referenced.*/
		CachedSubsystem = CachedSubsystem ? CachedSubsystem : GetWorld()->GetSubsystem<UReacousticSubsystem>();
		
		EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());
		SurfaceSoundAsset = CachedSubsystem->GetSurfaceSoundAsset(SurfaceType);
		
		OnComponentHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
	
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

void UReacousticComponent::TransferData(UReacousticSoundAsset* SoundDataAsset, UReacousticSoundAssociationMap* ReferenceMap)
{
	if(SoundDataAsset && ReferenceMap)
	{
		ReacousticSoundAsset = SoundDataAsset;
		ReacousticSoundAssociationMap = ReferenceMap;
	}
	else
	{
		UE_LOG(LogReacousticComponent,Warning,TEXT("Warning, failed to trasfer data to %s"),*GetOwner()->GetName());
	}
	
}

inline static float GetArraySum(const TArray<float>& Array)
{
	float Sum {0.0};
	for (const float Element : Array)
	{
		Sum += Element;
	}
	return Sum;
}

/** Filter the hit events so that the system only triggers at appropriate impacts.*/
//TODO: i'm passing a lot of values that i might not need. So i'll delete them later.
bool UReacousticComponent::FilterImpact(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	bool HitIsValid{false};
	ImpactForce = CalculateImpactValue(NormalImpulse,HitComp,OtherActor);
	/** We perform a lot of filtering to prevent hitsounds from playing in unwanted situations.*/
	if( ImpactForce > 30)
	{
		DeltaLocationDistance = abs(FVector::Distance(LatestLocation, Hit.Location));
		LatestLocation = Hit.ImpactPoint;
		
		/** prevent situations were sounds are hittng in the same location.*/
		if(DeltaLocationDistance > 1.5)
		{
			/** prevent situations were sounds are hittng in a short timespan.*/
			DeltaHitTime = abs(FMath::Clamp((FPlatformTime::Seconds() - LatestTime), 0.0, 10.0));
			LatestTime = FPlatformTime::Seconds();

			/** If there's a considerable time between hits, remove the hit state history.*/
			if(DeltaHitTime > 1.0)
			{
				DeltaStateArray.Empty();
			}
			
			if(DeltaHitTime > 0.05)
			{
				/** prevent situations were sounds are hittng in the same orientation.*/
				DeltaDirectionVector =
					abs(FVector::Distance(LatestForwardVector,this->GetOwner()->GetActorForwardVector())) +
					abs(FVector::Distance(LatestRightVector,this->GetOwner()->GetActorRightVector())) +
					abs(FVector::Distance(LatestUpVector,this->GetOwner()->GetActorUpVector()))
				;
				LatestForwardVector =this->GetOwner()->GetActorForwardVector();
				LatestRightVector = this->GetOwner()->GetActorRightVector();
				LatestUpVector = this->GetOwner()->GetActorUpVector();
				
				if(DeltaDirectionVector >0.1)
				{
					/** Prevent more erratic hits happening for a long time in the same location. Eg: An object glitching behind the wall.*/
					DeltaStateArray.Add(DeltaLocationDistance * DeltaHitTime + 100*DeltaDirectionVector);
					UE_LOG(LogReacousticComponent, Verbose, TEXT("DeltaStateArray Array Sum: %f"),(GetArraySum(DeltaStateArray)));
					if(GetArraySum(DeltaStateArray) > 0.5f || DeltaStateArray.Num() <= 20)
					{
							HitIsValid = true;
					}
					else{UE_LOG(LogReacousticComponent,Verbose,TEXT("Prevented hit by: STATE ARRAY"))}
				}
				else{UE_LOG(LogReacousticComponent,Verbose,TEXT("Prevented hit by: DELTA FORWARD VECTOR"))}
			}
			else{UE_LOG(LogReacousticComponent,Verbose,TEXT("Prevented hit by: DELTA HIT TIME"))}
		}
		else{UE_LOG(LogReacousticComponent,Verbose,TEXT("Prevented hit by: LOCATION DISTANCE"))}
		
		/** Remove the oldest delta location distance from the array if it exceeds the limit. */
		if (DeltaStateArray.Num() > 20)
		{
			DeltaStateArray.RemoveAt(0);
		}
	}
	return HitIsValid;
}


FVector2D UReacousticComponent::ReturnTimeStampWithStrenght(UReacousticSoundAsset* SoundAsset, float ImpactValue)
{
	/** Make sure the subsystem is still referenced.*/
	CachedSubsystem = CachedSubsystem ? CachedSubsystem : GetWorld()->GetSubsystem<UReacousticSubsystem>();

	/** Use the reacoustic subsystem to pass on the timestamp and strength value.*/
	return CachedSubsystem->GetTimeStampWithStrenght(SoundAsset, ImpactValue);

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


