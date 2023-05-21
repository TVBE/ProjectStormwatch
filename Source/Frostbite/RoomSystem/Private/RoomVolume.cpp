// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#include "RoomVolume.h"
#include "LogCategories.h"
#include "RoomComponent.h"
#include "Components/ShapeComponent.h"

ARoomVolume::ARoomVolume()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ARoomVolume::PostInitProperties()
{
	Super::PostInitProperties();

	if (UShapeComponent* ShapeComponent {GetCollisionComponent()})
	{
		ShapeComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
		ShapeComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ShapeComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	}
}

void ARoomVolume::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	TArray<UActorComponent*> Components;
	GetComponents(Components);

	for (UActorComponent* Component : Components)
	{
		if (URoomComponent* RoomComponent = Cast<URoomComponent>(Component))
		{
			RoomComponent->ConstructComponent();
		}
	}
}

void ARoomVolume::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
	if (APawn* OverlappingPawn {Cast<APawn>(OtherActor)})
	{
		OverlappingPawns.AddUnique(OverlappingPawn);
		OnPawnEnter.Broadcast(OverlappingPawn);
		UE_LOG(LogRoomVolume, Verbose, TEXT("Pawn '%s' has entered room '%s'."), *OtherActor->GetName(), *this->GetName());
	}
}

void ARoomVolume::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);
	
	if (APawn* OverlappingPawn {Cast<APawn>(OtherActor)})
	{
		OverlappingPawns.Remove(OverlappingPawn);
		OnPawnLeave.Broadcast(OverlappingPawn);
		UE_LOG(LogRoomVolume, Verbose, TEXT("Pawn '%s' has left room '%s'."), *OtherActor->GetName(), *this->GetName());
	}
}



