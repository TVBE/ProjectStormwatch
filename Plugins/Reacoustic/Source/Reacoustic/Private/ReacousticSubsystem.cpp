// Copyright 2023 Nino Saglia & Tim Verberne

#include "ReacousticSubsystem.h"
#include "ReacousticComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SceneComponent.h"
#include "ReacousticDataTypes.h"







UReacousticSubsystem::UReacousticSubsystem()
{
	Settings = GetMutableDefault<UReacousticProjectSettings>();

	//TODO: get all the settings from the project settngs and use them to populate wold with reacoustic objects without any functions in the level blueprint.
}

void UReacousticSubsystem::PostInitProperties()
{
	
	if(!ReacousticSoundDataAsset || !UReacousticSoundDataRefMap)
	{
		UE_LOG(LogAssetData, Error, TEXT("Failed to instance data assets."))
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
	UE_LOG(LogReacousticSubsystem, Warning, TEXT("On Actor spawned"));
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
void UReacousticSubsystem::AddBPReacousticComponentToActor(AActor* Actor, TSubclassOf<UReacousticComponent> ComponentClass, FReacousticSoundData MeshSoundData)
{
	if (!UReacousticSoundDataRefMap || !ReacousticSoundDataAsset)
	{
		return;
	}
	
    UActorComponent* NewComponent {nullptr};
    if (!Actor)
    {
        UE_LOG(LogReacousticComponent, Warning, TEXT("AddComponentToActor was called without passing an actor pointer."))
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
            ReacousticComponent->TransferData(ReacousticSoundDataAsset, UReacousticSoundDataRefMap, MeshSoundData);
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

FReacousticSoundData UReacousticSubsystem::ReturnSoundDataForMesh(UStaticMeshComponent* StaticMeshComponent)
{
	/** Find the ReacousticSoundDataRef for the StaticMeshComponent. */
	for (const auto& [Mesh, SoundDataRef] : UReacousticSoundDataRefMap->MeshMapEntries)
	{
		/** Get the mesh and associated sound data reference from the UReacousticSoundDataRefMap. */
		UStaticMesh* MeshPtr {Mesh};
		const int32 SoundDataRefValue {SoundDataRef};

		/** If the current mesh is the same as the StaticMeshComponent's mesh, retrieve the row number that matches the sound data reference. */
		if (StaticMeshComponent->GetStaticMesh() == MeshPtr)
		{
			return ReacousticSoundDataAsset->AudioData[SoundDataRefValue];
		}
	}
	/** If no matching mesh was found, return the default sound data.*/
	return ReacousticSoundDataAsset->AudioData[0];
}

/** For current use: adds a blueprint component derrived from the reacoustics c++ component. */
void UReacousticSubsystem::PopulateWorldWithBPReacousticComponents(TSubclassOf<class UReacousticComponent> ComponentClass)
{
	if (!ComponentClass)
	{
		UE_LOG(LogReacousticSubsystem, Warning, TEXT("ComponentClass is nullptr."));
		return;
	}

	if (!UReacousticSoundDataRefMap || !ReacousticSoundDataAsset)
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

			if (UStaticMeshComponent* StaticMeshComponent {Cast<UStaticMeshComponent>(ActorComponent)})
			{
				AddBPReacousticComponentToActor(Actor, ComponentClass, ReturnSoundDataForMesh(StaticMeshComponent));
			}
		}
	}
}


