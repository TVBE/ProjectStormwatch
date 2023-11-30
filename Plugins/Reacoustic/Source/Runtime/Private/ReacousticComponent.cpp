// Copyright (c) 2022-present Nino Saglia. All Rights Reserved.
// Written by Nino Saglia.
#include "ReacousticComponent.h"
#include "ReacousticSoundAsset.h"

DEFINE_LOG_CATEGORY_CLASS(UReacousticComponent, LogReacousticComponent);

UReacousticComponent::UReacousticComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UReacousticComponent::BeginPlay()
{
	Super::BeginPlay();


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

	
	UReacousticSoundAsset* AssetToTransfer = ReacousticSoundAsset ? ReacousticSoundAsset : GetReacousticSubsystem()->GetMeshSoundAsset(MeshComponent);

	if(!AssetToTransfer)
	{
		// Handle this case. Maybe log an error and return?
		UE_LOG(LogReacousticComponent, Error, TEXT("AssetToTransfer is null!"));
		return;
	}
	
	TransferData(AssetToTransfer, GetReacousticSubsystem()->ReacousticSoundAssociationMap);
	
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
	
	/** Pass All relevant parameters for startup to the sound component.*/
	if(ReacousticSoundAsset->Sound_Attenuation){AudioComponent->AttenuationSettings = ReacousticSoundAsset->Sound_Attenuation;}
	if(ReacousticSoundAsset->Sound){AudioComponent->SetObjectParameter(TEXT("Obj_WaveAsset"), ReacousticSoundAsset->Sound);}
	if(ReacousticSoundAsset->ImpulseLength){AudioComponent->SetFloatParameter(TEXT("Obj_Length"), ReacousticSoundAsset->ImpulseLength);}
	USoundBase* SoundObject = Cast<USoundBase>(StaticLoadObject(UObject::StaticClass(), nullptr, TEXT("/Reacoustic/Audio/Metasounds/MSS_Reacoustic"))); //Asset path referencing for now. I might include Metasound plugin later.
	AudioComponent->SetSound(SoundObject);
	
	/** Add the audio component to the parent actor */
	AudioComponent->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
}

float UReacousticComponent::CalculateImpactValue(const FVector& NormalImpulse, const UPrimitiveComponent* HitComponent, const AActor* OtherActor) {
	if (!HitComponent || !HitComponent->IsSimulatingPhysics() || !OtherActor) {
		return 0.0f;
	}

	const FVector RelativeVelocity {HitComponent->GetComponentVelocity() - OtherActor->GetVelocity()};
	// Rotation is currently not used since it results in unpredictable sound.
	// const float RotationalSpeed = FMath::Abs(HitComponent->GetMass()*HitComponent->GetPhysicsAngularVelocityInRadians().Length());
	const FVector ScaledImpulse {(NormalImpulse + RelativeVelocity)};
	const float D = FMath::Abs(FVector::DotProduct(RelativeVelocity.GetSafeNormal(), NormalImpulse.GetSafeNormal()));
	return (RelativeVelocity.Length() * D * 0.1); 
}

UReacousticSubsystem* UReacousticComponent::GetReacousticSubsystem()
{
	if(!CachedSubsystem)
	{
		if (const UWorld* World = GetWorld())
		{
			CachedSubsystem = World->GetSubsystem<UReacousticSubsystem>();
		}
	}
	check(CachedSubsystem);
	return CachedSubsystem;
}


void UReacousticComponent::HandleOnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if(FilterImpact(HitComp,OtherActor,OtherComp,NormalImpulse,Hit))
	{
		/** Get the surface sound for this impact.*/
		EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());
		SurfaceSoundAsset = GetReacousticSubsystem()->GetSurfaceSoundAsset(SurfaceType);
		if(ReacousticSoundAsset)
		{
			float ImpactValue = CalculateImpactValue(NormalImpulse,HitComp,OtherActor)/ReacousticSoundAsset->MaxSpeedScalar;
		
			FImpactValueToTimestampResult Result {GetReacousticSubsystem()->GetTimeStampWithStrenght(ReacousticSoundAsset,ImpactValue)};
			UE_LOG(LogReacousticComponent, Verbose, TEXT("Impact Strenght: %f"),ImpactValue);
			UE_LOG(LogReacousticComponent, Verbose, TEXT("Impact Timestamp Result: %f"),Result.Timestamp);
			UE_LOG(LogReacousticComponent, Verbose, TEXT("Normalized Strenght: %f"),Result.NormalizedOnsetStrength);
		
			/** Pass all relevant parameters to the audio component for audio playback.*/
			AudioComponent->SetFloatParameter(TEXT("Obj_StartTime"), Result.Timestamp);
			AudioComponent->SetFloatParameter(TEXT("Obj_Velocity"), ImpactValue);
			AudioComponent->SetObjectParameter(TEXT("Obj_WaveAsset"), ReacousticSoundAsset->Sound);
			AudioComponent->SetFloatParameter(TEXT("Obj_Length"), ReacousticSoundAsset->ImpulseLength);
		}
		
		/** For blueprint implementation.*/
		OnComponentHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
		
		/** The metasound can handle polyphonic hits by retriggering the sound using this parameter.*/
		if(AudioComponent->IsPlaying())
		{
			AudioComponent->SetTriggerParameter("TriggerSound");
		}
		else
		{
			AudioComponent->Play();
		}
	}
}




/** This implementation will likely  be overridden by a blueprint or function that needs to trigger a custom hit.*/
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
bool UReacousticComponent::FilterImpact(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	bool HitIsValid{false};
	ImpactForce = CalculateImpactValue(NormalImpulse, HitComp, OtherActor);

	if (ImpactForce <= .5) {
		return HitIsValid;//false
	}

	auto IsDifferentLocation = [&]() {
		DeltaLocationDistance = abs(FVector::Distance(LatestLocation, Hit.Location));
		LatestLocation = Hit.ImpactPoint;
		return DeltaLocationDistance > 0.05;
	};

	auto HasSufficientTimeBetweenHits = [&]() {
		DeltaHitTime = abs(FMath::Clamp((FPlatformTime::Seconds() - LatestTime), 0.0, 10.0));
		if (DeltaHitTime > 1.0) {
			DeltaStateArray.Empty();
		}
		if (DeltaHitTime > 0.01)
		{
			LatestTime = FPlatformTime::Seconds();
		}

		return DeltaHitTime > 0.1;
	};

	auto IsDifferentOrientation = [&]() {
		DeltaDirectionVector =
			abs(FVector::Distance(LatestForwardVector, this->GetOwner()->GetActorForwardVector())) +
			abs(FVector::Distance(LatestRightVector, this->GetOwner()->GetActorRightVector())) +
			abs(FVector::Distance(LatestUpVector, this->GetOwner()->GetActorUpVector()));

		LatestForwardVector = this->GetOwner()->GetActorForwardVector();
		LatestRightVector = this->GetOwner()->GetActorRightVector();
		LatestUpVector = this->GetOwner()->GetActorUpVector();

		return DeltaDirectionVector > 0.05;
	};

	auto IsValidHitState = [&]() {
		DeltaStateArray.Add(DeltaLocationDistance * DeltaHitTime + 100 * DeltaDirectionVector);
		UE_LOG(LogReacousticComponent, Verbose, TEXT("DeltaStateArray Array Sum: %f"), (GetArraySum(DeltaStateArray)));
		return GetArraySum(DeltaStateArray) > 0.1f;
	};

	if (!IsDifferentLocation()) {
		UE_LOG(LogReacousticComponent, Verbose, TEXT("Prevented hit by: LOCATION DISTANCE"));
	} 
	
	else if (!HasSufficientTimeBetweenHits()) {
		UE_LOG(LogReacousticComponent, Verbose, TEXT("Prevented hit by: DELTA HIT TIME"));
	} 
	
	else if (!IsDifferentOrientation()) {
		UE_LOG(LogReacousticComponent, Verbose, TEXT("Prevented hit by: DELTA FORWARD VECTOR"));
	} 
	
	else if (IsValidHitState()) {
		HitIsValid = true;
	} 
	
	else {
		UE_LOG(LogReacousticComponent, Verbose, TEXT("Prevented hit by: STATE ARRAY"));
	}

	if (DeltaStateArray.Num() > 20) {
		DeltaStateArray.RemoveAt(0);
	}

	return HitIsValid;
}



FImpactValueToTimestampResult UReacousticComponent::ReturnTimeStampWithStrenght(UReacousticSoundAsset* SoundAsset, float ImpactValue)
{
	return GetReacousticSubsystem()->GetTimeStampWithStrenght(SoundAsset, ImpactValue);

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


