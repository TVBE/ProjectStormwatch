// RoomComponentBase.cpp

#include "RoomComponentBase.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"

URoomComponentBase::URoomComponentBase()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URoomComponentBase::BeginPlay()
{
	Super::BeginPlay();

	/** Set the ParentBoxCollider to the first UBoxComponent found in the parent actor */
	TArray<UBoxComponent*> BoxComponents;
	GetOwner()->GetComponents<UBoxComponent>(BoxComponents);
	if (BoxComponents.Num() > 0)
	{
		ParentBoxCollider = BoxComponents[0];
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No UBoxComponent found in parent actor for RoomComponentBase %s"), *GetOwner()->GetName());
	}

	if (ParentBoxCollider)
	{
		BindOverlapEvents();
		
		/** Check if player is already inside collider */
		TArray<AActor*> OverlappingActors;
		ParentBoxCollider->GetOverlappingActors(OverlappingActors, ACharacter::StaticClass());
		PlayerInsideCollider = OverlappingActors.Num() > 0;
		if(PlayerInsideCollider)
		{
			DoSomethingOnBeginOverlap();
		}
		
	}

	
}



void URoomComponentBase::BindOverlapEvents()
{
	ParentBoxCollider->OnComponentBeginOverlap.AddDynamic(this, &URoomComponentBase::OnOverlapBegin);
	ParentBoxCollider->OnComponentEndOverlap.AddDynamic(this, &URoomComponentBase::OnOverlapEnd);
}

void URoomComponentBase::UnbindOverlapEvents()
{
	ParentBoxCollider->OnComponentBeginOverlap.RemoveDynamic(this, &URoomComponentBase::OnOverlapBegin);
	ParentBoxCollider->OnComponentEndOverlap.RemoveDynamic(this, &URoomComponentBase::OnOverlapEnd);
}

void URoomComponentBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (OtherActor->IsA<ACharacter>())
		{
			PlayerInsideCollider = true;
			if (Enabled)
			{
				DoSomethingOnBeginOverlap();
			}
		}
}


void URoomComponentBase::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA<ACharacter>())
	{
		PlayerInsideCollider = false;
		if (Enabled)
		{
			DoSomethingOnEndOverlap();
		}
	}
}

void URoomComponentBase::OnActorSpawned(AActor* SpawnedActor)
{
	if (SpawnedActor->IsA<ACharacter>())
	{
		/** Check if player is inside collider and do something if necessary */
		if (PlayerInsideCollider && Enabled)
		{
			if(Enabled)
			{
				DoSomethingOnBeginOverlap();
			}
		}
	}
}

void URoomComponentBase::DoSomethingOnBeginOverlap()
{
	/** Implement the desired behavior here */
	UE_LOG(LogTemp, Warning, TEXT("Player has overlapped %s"), *ParentBoxCollider->GetName());
}

void URoomComponentBase::DoSomethingOnEndOverlap()
{
	/** Implement the desired behavior here */
	UE_LOG(LogTemp, Warning, TEXT("Player has stopped overlapping %s"), *ParentBoxCollider->GetName());
}