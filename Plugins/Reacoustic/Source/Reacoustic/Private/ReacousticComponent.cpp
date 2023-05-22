// Copyright 2023 Nino Saglia & Tim Verberne

#include "ReacousticComponent.h"
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
				bool bFoundCompatibleComponent = false;
				for (UActorComponent* Component : Components)
				{
					if (UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(Component))
					{
						if (StaticMeshComponent->IsSimulatingPhysics() && StaticMeshComponent->BodyInstance.bNotifyRigidBodyCollision)
						{
							MeshComponent = StaticMeshComponent;
							// StaticMeshComponent->SetGenerateOverlapEvents(true);
							bFoundCompatibleComponent = true;
							break;
						}
					}
				}

				if (!bFoundCompatibleComponent)
				{
					UE_LOG(LogReacousticComponent, Warning, TEXT("Reacoustic component is not compatible with the parent actor: %s. Deleting it."),*FString(this->GetOwner()->GetName()));
					GetOwner()->RemoveOwnedComponent(this);
					this->UnregisterComponent();
					AudioComponent->UnregisterComponent();
					AudioComponent->DestroyComponent();
					this->DestroyComponent();
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
	if(!AudioComponent)
	{
		AudioComponent = NewObject<UAudioComponent>(GetOwner());
		AudioComponent->RegisterComponent();
		AudioComponent->SetSound(SoundBase);
	}
	/** Set the attenuation settings */
	AudioComponent->AttenuationSettings = MeshAudioData.Attenuation;

	/** Set the parameters.*/
	AudioComponent->SetFloatParameter(TEXT("Obj_Length"), MeshAudioData.ImpulseLength);
	AudioComponent->SetWaveParameter(TEXT("Obj_WaveAsset"), MeshAudioData.ImpactWaveAsset);

	/** Add the audio component to the parent actor */
	AudioComponent->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
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
	if(Hit.ImpactNormal.Length() > 0.00001)
	{
		DeltaLocationDistance = abs(FVector::Distance(LatestLocation, Hit.Location));
		
		
		LatestLocation = Hit.ImpactPoint;
		
		
		
		/** prevent situations were sounds are hittng in the same location.*/
		if(DeltaLocationDistance > 0.001)
		{
			/** prevent situations were sounds are hittng in a short timespan.*/
			DeltaHitTime = abs(FMath::Clamp((FPlatformTime::Seconds() - LatestTime), 0.0, 1.0));
			LatestTime = FPlatformTime::Seconds();

			/** If there's a considerable time between hits, remove the hit history.*/
			if(DeltaHitTime > 0.3)
			{
				DeltaStateArray.Empty();
			}
			
			if(DeltaHitTime > 0.01)
			{
				/** prevent situations were sounds are hittng in the same orientation.*/
				DeltaForwardVector =abs(FVector::Distance(LatestForwardVector,this->GetOwner()->GetActorForwardVector()));
				LatestForwardVector =this->GetOwner()->GetActorForwardVector();
				
				if(DeltaForwardVector >0.01)
				{
					/** Prevent more erratic hits happening for a long time in the same location. Eg: An object glitching behind the wall.*/

					DeltaStateArray.Add(DeltaLocationDistance * DeltaHitTime + DeltaForwardVector);
					UE_LOG(LogReacousticComponent, Verbose, TEXT("DeltaStateArray Array Sum: %f"),(GetArraySum(DeltaStateArray)));
					if(GetArraySum(DeltaStateArray) > 10.0f || DeltaStateArray.Num() <= 5)
					{
							// Calculate the impact force
							ImpactForce = NormalImpulse.Size(); // HitComp->GetMass();
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

		if(MeshAudioData.OnsetVolumeData.Num() == 0)
		{
			if(const UWorld* World {GetWorld()})
			{
				if(UReacousticSubsystem* Subsystem {World->GetSubsystem<UReacousticSubsystem>()})
				{
					TransferData(Subsystem->ReacousticSoundDataAsset,Subsystem->UReacousticSoundDataRefMap,Subsystem->GetMeshSoundData(MeshComponent));
					if(Subsystem->GetMeshSoundData(MeshComponent).OnsetVolumeData.Num() == 0)
					{
						UE_LOG(LogReacousticComponent, Warning, TEXT("Reacoustic component couldn't find any valid sounds for %s. Deleting it."),*FString(this->GetOwner()->GetName()));
						GetOwner()->RemoveOwnedComponent(this);
						this->UnregisterComponent();
						AudioComponent->UnregisterComponent();
						AudioComponent->DestroyComponent();
						this->DestroyComponent();
					}
					Initialize();
				}
			}
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
	else
	{
		UE_LOG(LogReacousticComponent,Warning,TEXT("Warning, failed to trasfer data to %s"),*GetOwner()->GetName());
	}
	
}



int UReacousticComponent::CalcluateSoundDataEntry(FReacousticSoundData SoundData, float ImpactValue)
{
	/** data list is sorted and normalized so that the normalized inpact value * timing data returns the correct sample location that matches the interaction.*/
	int DataEntryIn{ int(ImpactValue * SoundData.OnsetTimingData.Num())};
	
	/** if the following loop can't find a new entry, just return the entry without filtering. */
	int DataEntryOut{DataEntryIn};
	
	/** if the interval is quite large, then forget the latest hit results.*/
	if(DeltaHitTime > 0.5)
	{
		LatestSoundTimings.Reset();
	}
	/** Iterate over SoundData.OnsetTimingData from the intended timestamp. */
	if(SoundData.OnsetTimingData.Num())
	{
		for (int32 i = 0; i < SoundData.OnsetTimingData.Num(); i++)
		{
			/** iterate in a pattern of: DataEntryIn +1, -1, +2, -2 etc...*/
			int32 offset{DataEntryIn + i};
			int32 index{DataEntryIn + offset * (offset % 2 == 0 ? 1 : -1)};
			int32 indexClamped{FMath::Clamp(index,0,SoundData.OnsetTimingData.Num()-1)};
        
			if (indexClamped >= 0 && indexClamped < SoundData.OnsetTimingData.Num())
			{
				float TimingToTest = SoundData.OnsetTimingData[indexClamped];

				if(LatestSoundTimings.Num() == 0)
				{
					LatestSoundTimings.Add(TimingToTest);
					DataEntryOut = TimingToTest;
				}
				else
				{
					for (int32 j = 0; j < LatestSoundTimings.Num(); j++)
					{
						float LatestSoundTiming = LatestSoundTimings[j];
						bool FoundValidTimeStamp{true};
						if (FMath::Abs(TimingToTest - LatestSoundTiming) < SoundData.ImpulseLength)
						{
							FoundValidTimeStamp = false;
							break;
						}
						if(FoundValidTimeStamp)
						{
							LatestSoundTimings.Add(TimingToTest);
							DataEntryOut = TimingToTest;
							break;
						}
					}
				}
			
			}
		}
	}

	if (LatestSoundTimings.Num() > 10)
	{
		LatestSoundTimings.RemoveAt(0);
	}
	return DataEntryOut;
}


