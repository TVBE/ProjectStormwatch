// Copyright (c) 2022-present Nino Saglia. All Rights Reserved.
// Written by Nino Saglia.

#include "ReacousticSubsystem.h"
#include "ReacousticComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SceneComponent.h"
#include "ReacousticDataTypes.h"

DEFINE_LOG_CATEGORY_CLASS(UReacousticSubsystem, LogReacousticSubsystem);

UReacousticSubsystem::UReacousticSubsystem()
{
	Settings = GetMutableDefault<UReacousticProjectSettings>();

	//TODO: get all the settings from the project settngs and use them to populate wold with reacoustic objects without any functions in the level blueprint.
}

void UReacousticSubsystem::PostInitProperties()
{
	
	if(!ReacousticSoundAssociationMap)
	{
		
	}
	
	Super::PostInitProperties();
	
	UWorld* World = GetWorld();
	if (World)
	{
		World->AddOnActorSpawnedHandler(FOnActorSpawned::FDelegate::CreateUObject(this, &UReacousticSubsystem::OnActorSpawned));
	}
}


void UReacousticSubsystem::OnActorSpawned(AActor* Actor)
{
	
}

void UReacousticSubsystem::RegisterComponent(UReacousticComponent* Component)
{
	ReacousticComponents.AddUnique(Component);
}

void UReacousticSubsystem::UnregisterComponent(UReacousticComponent* Component)
{
	ReacousticComponents.Remove(Component);
}

bool UReacousticSubsystem::IsReacousticCompatible(AActor* Actor)
{
	if (Actor->GetRootComponent() && Actor->GetRootComponent()->IsSimulatingPhysics())
	{
		TArray<UStaticMeshComponent*> Components;
		Actor->GetComponents<UStaticMeshComponent>(Components);
		if(!Components.IsEmpty())
		{
			for(UStaticMeshComponent* Component : Components)
			{
				if(Component->BodyInstance.bNotifyRigidBodyCollision)
				{
					return true;
				}
			}
		}
	}
	return false;
}

/* Adds a reacousticComponent derived component to an actor. and returns the pointer to this component.*/
void UReacousticSubsystem::AddBPReacousticComponentToActor(AActor* Actor, TSubclassOf<UReacousticComponent> ComponentClass, UReacousticSoundAsset* MeshSoundAsset)
{
	if (!ReacousticSoundAssociationMap)
	{
		return;
	}
	
    UActorComponent* NewComponent {nullptr};
    if (!Actor)
    {
        UE_LOG(LogReacousticSubsystem, Warning, TEXT("AddComponentToActor was called without passing an actor pointer."))
        return;
    }
    if (Actor->GetComponentByClass(ComponentClass))
    {
        const FString ActorName{Actor->GetActorNameOrLabel()};
        UE_LOG(LogReacousticSubsystem, Log, TEXT("Skipped adding a component to %s because this actor already has a component of the same class attached to it."), *ActorName);
    }
    else
    {
        NewComponent = Actor->AddComponentByClass(ComponentClass, false, FTransform(), true);
    }
    if(NewComponent)
    {
        if(UReacousticComponent* ReacousticComponent {Cast<UReacousticComponent>(NewComponent)})
        {
            ReacousticComponent->RegisterComponent();
        }
    }
}

TArray<AActor*> UReacousticSubsystem::GetCompatibleActorsOfClass(UClass* ClassType)
{

    TArray<AActor*> Array{};
    const UWorld* World{GetWorld()};
    if (!World || !ClassType)
    {
        UE_LOG(LogReacousticSubsystem, Log, TEXT("Error world or class is invalid"));
        return Array;
    }
    TArray<AActor*> FoundActors{};
    UGameplayStatics::GetAllActorsOfClass(World, ClassType, FoundActors);
    const int TotalCount{FoundActors.Num()};
    int CompatibleCount{0};
    for (AActor* Actor : FoundActors)
    {
        if(IsReacousticCompatible(Actor))
        {
            Array.Add(Actor);
            CompatibleCount++;
        }
    }
    const FString Name{ClassType->GetName()};
    UE_LOG(LogReacousticSubsystem, Log, TEXT("Found %d actors of type %s out of which %d can be used by the Reacoustic system."), TotalCount, *Name, CompatibleCount);
    return Array;
}


/** For current use: adds a blueprint component derrived from the reacoustics c++ component. */
void UReacousticSubsystem::PopulateWorldWithBPReacousticComponents(TSubclassOf<class UReacousticComponent> ComponentClass)
{
	if (!ComponentClass)
	{
		UE_LOG(LogReacousticSubsystem, Warning, TEXT("ComponentClass is nullptr."));
		return;
	}

	if (!ReacousticSoundAssociationMap)
	{
		return;
	}

	TArray<AActor*> CompatibleActors {GetCompatibleActorsOfClass(AStaticMeshActor::StaticClass())};
	for (AActor* Actor : CompatibleActors)
	{
		if (!Actor)
		{
			continue;
		}

		TArray<UActorComponent*> Components{};
		Actor->GetComponents(Components);
		for (UActorComponent* ActorComponent : Components)
		{
			if (!ActorComponent)
			{
				continue;
			}

			if (const UStaticMeshComponent* StaticMeshComponent {Cast<UStaticMeshComponent>(ActorComponent)})
			{

				AddBPReacousticComponentToActor(Actor, ComponentClass, GetMeshSoundAsset(StaticMeshComponent));
			}
		}
	}
}

UReacousticSoundAsset* UReacousticSubsystem::GetMeshSoundAsset(const UStaticMeshComponent* StaticMeshComponent) const
{
	if (!StaticMeshComponent || !ReacousticSoundAssociationMap)
	{
		return nullptr;
	}

	for (const auto& [Mesh, SoundAsset] : ReacousticSoundAssociationMap->MeshMapEntries)
	{
		UStaticMesh* MeshPtr {Mesh};

		if (StaticMeshComponent->GetStaticMesh() == MeshPtr)
		{
			return SoundAsset;
		}
	}

	return nullptr;
}

UReacousticSoundAsset* UReacousticSubsystem::GetSurfaceSoundAsset(const EPhysicalSurface SurfaceIn) const
{
	if (!ReacousticSoundAssociationMap)
	{
		return nullptr;
	}

	for (const auto& [Surface, SoundAsset] : ReacousticSoundAssociationMap->PhysicalMaterialMapEntries)
	{


		if (SurfaceIn == Surface)
		{
			return SoundAsset;
		}
	}

	return nullptr;
}

FVector2D UReacousticSubsystem::GetTimeStampWithStrenght(UReacousticSoundAsset* SoundAsset, float ImpactValue)
{
	float BestDifference = FLT_MAX; 
	float BestTimeStamp = -1;
	float BestStrenght = -1;

	TArray<float> OutOnsetTimestamps;
	TArray<float> OutOnsetStrengths;
	
	if (!SoundAsset || !SoundAsset->Sound)
	{
		return FVector2D(0,0);
	}

	SoundAsset->GetNormalizedChannelOnsetsBetweenTimes(0,SoundAsset->Sound->GetDuration(),0,OutOnsetTimestamps, OutOnsetStrengths);
	/** Iterate over TMap and find the key asociated with the volume value closest to impact value */
	if (OutOnsetTimestamps.Num() == OutOnsetStrengths.Num())
	{
		for (int i = 0; i < OutOnsetTimestamps.Num(); ++i)
		{
			float timestamp = OutOnsetTimestamps[i];
			float strength = OutOnsetStrengths[i];
			
			/** If this element is in LatestMatchingElements, skip to the next iteration */
			/** Iterate throug the recorded sound timestamps.*/
			bool next{false};
			for (int32 j = 0; j < SoundAsset->GetTimestampHistory().Num(); j++)
			{
				float LatestMatchingTimestamp = SoundAsset->GetTimestampHistory()[j];
				/** if any of them don't fit, break and test the next timestamp.*/
				if (FMath::Abs(timestamp - LatestMatchingTimestamp) < SoundAsset->ImpulseLength*2)
				{
					next = true;
					break;
				}
			}
			if(next)
			{
				continue;
			}
			const float CurrentDifference = FMath::Abs(ImpactValue - strength);
			if(CurrentDifference < BestDifference)
			{
				BestDifference = CurrentDifference;
				BestTimeStamp = timestamp;
				BestStrenght = strength;
			}
		}
	}

	/** If we found a matching timestamp, update LatestMatchingElements */
	if(BestTimeStamp != -1)
	{
		SoundAsset->UpdateTimestampHistory(BestTimeStamp);
	}
	return FVector2D(BestTimeStamp, BestStrenght);
}