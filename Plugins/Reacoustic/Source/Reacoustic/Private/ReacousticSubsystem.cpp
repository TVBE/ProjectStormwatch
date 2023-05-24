// Copyright (c) 2022-present Nino Saglia. All Rights Reserved.
// Written by Nino Saglia.

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

			if (const UStaticMeshComponent* StaticMeshComponent {Cast<UStaticMeshComponent>(ActorComponent)})
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
						const FReacousticSoundData MeshSoundData {ReacousticSoundDataAsset->AudioData[SoundDataRefValue]};
						AddBPReacousticComponentToActor(Actor, ComponentClass, MeshSoundData);
					}
				}
			}
		}
	}
}

