// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#include "MeshInteractionComponent.h"
#include "CollisionQueryParams.h"
#include "Engine/StaticMeshActor.h"

void UMeshInteractionComponent::OnRegister()
{
	Super::OnRegister();

	UStaticMeshComponent* MeshComponent;
	
	if (const AStaticMeshActor* MeshActor {Cast<AStaticMeshActor>(GetOwner())})
	{
		MeshComponent = MeshActor->GetStaticMeshComponent();
	}
	else
	{
		MeshComponent = Cast<UStaticMeshComponent>(GetOwner()->FindComponentByClass(UStaticMeshComponent::StaticClass()));
	}
	
	if (MeshComponent)
	{
		MeshComponent->SetMobility(EComponentMobility::Movable);
		MeshComponent->SetSimulatePhysics(true);
		MeshComponent->PutRigidBodyToSleep();
		MeshComponent->SetCollisionProfileName(TEXT("InteractableMesh"));
		
		MeshComponent->SetGenerateOverlapEvents(true);

		// constexpr float MinBoundingBoxVolume {1000000.0f};
		constexpr float MinMass {12.0f};

		// const FVector BoxExtent {MeshComponent->Bounds.BoxExtent};

		/** 8 = 2^3, since BoxExtent represents half of the bounding box size. */
		// const float BoundingBoxVolume {static_cast<float>(BoxExtent.X * BoxExtent.Y * BoxExtent.Z * 8)};

		const float Mass {MeshComponent->GetMass()};
		
		if (Mass > MinMass)
		{
			MeshComponent->SetCollisionObjectType(ECC_PhysicsBody);
		}
		else
		{
			MeshComponent->SetCollisionObjectType(ECC_GameTraceChannel2);
			MeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
			
			MeshComponent->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;

			MeshComponent->SetNotifyRigidBodyCollision(true);
		}
	}
}

void UMeshInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	if (const AStaticMeshActor* MeshActor {Cast<AStaticMeshActor>(GetOwner())})
	{
		if (UStaticMeshComponent* MeshComponent {MeshActor->GetStaticMeshComponent()})
		{
			MeshComponent->PutRigidBodyToSleep();
		}
	}
}
