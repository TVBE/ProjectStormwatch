// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "Stormwatch/Actors/Public/BHPhysicsMeshActor.h"

#include "ReacousticComponent.h"
#include "Misc/UObjectToken.h"

#define LOCTEXT_NAMESPACE "BHPhysicsMeshActor"

DEFINE_LOG_CATEGORY_STATIC(LogBHPhysicsMeshActor, Log, All)

ABHPhysicsMeshActor::ABHPhysicsMeshActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	if (StaticMeshComponent)
	{
		RootComponent = StaticMeshComponent;
		StaticMeshComponent->Mobility = EComponentMobility::Movable;
		StaticMeshComponent->SetSimulatePhysics(true);
	}
	
	ReacousticComponent = CreateDefaultSubobject<UReacousticComponent>(TEXT("ReacousticComponent"));
	if (ReacousticComponent)
	{
		ReacousticComponent->SetupAttachment(StaticMeshComponent);
	}
}

void ABHPhysicsMeshActor::BeginPlay()
{
	Super::BeginPlay();
}

void ABHPhysicsMeshActor::CheckForErrors()
{
	Super::CheckForErrors();

	if (!IsValid(StaticMeshComponent))
	{
		return;
	}
	if (!StaticMeshComponent->GetStaticMesh())
	{
		FFormatNamedArguments Arguments;
		Arguments.Add(TEXT("ActorName"), FText::FromString(GetName()));
		FMessageLog("MapCheck").Warning()
			->AddToken(FUObjectToken::Create(this))
			->AddToken(FTextToken::Create(FText::Format( LOCTEXT( "MapCheck_Message_InvalidStaticMesh",
				"{ActorName} does not have a Static Mesh assigned."), Arguments )));
	}
	if (!StaticMeshComponent->IsSimulatingPhysics())
	{
		FFormatNamedArguments Arguments;
		Arguments.Add(TEXT("ActorName"), FText::FromString(GetName()));
		Arguments.Add(TEXT("ComponentName"), FText::FromString(StaticMeshComponent->GetName()));
		FMessageLog("MapCheck").Warning()
			->AddToken(FUObjectToken::Create(this))
			->AddToken(FTextToken::Create(FText::Format( LOCTEXT( "MapCheck_Message_PhysicsSimulation",
				"{ActorName}::{ComponentName} is not configured to simulate physics."), Arguments )));
	}
}

UStaticMeshComponent* ABHPhysicsMeshActor::GetStaticMeshComponent() const
{
	return StaticMeshComponent;
}

UReacousticComponent* ABHPhysicsMeshActor::GetReacousticComponent() const
{
	return ReacousticComponent;
}

