// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#include "MeshInteractionComponent.h"
#include "CollisionQueryParams.h"
#include "Engine/StaticMeshActor.h"

void UMeshInteractionComponent::OnComponentCreated()
{
	Super::OnComponentCreated();

	if(IsAutoConfigurable)
	{
		
	}
	else
	{
		InteractionType = IsHeavy ? EInteractionType::Draggable : EInteractionType::Grabbable;
		InteractionHandType = IsLarge ? EInteractionHandType::TwoHanded : EInteractionHandType::OneHanded;
	}
}

void UMeshInteractionComponent::OnRegister()
{
	Super::OnRegister();
	if (const AStaticMeshActor* MeshActor {Cast<AStaticMeshActor>(GetOwner())})
	{
		if (UStaticMeshComponent* MeshComponent {MeshActor->GetStaticMeshComponent()})
		{
			MeshComponent->SetSimulatePhysics(true);
			MeshComponent->SetCollisionProfileName(TEXT("InteractableMesh"));
			MeshComponent->SetNotifyRigidBodyCollision(true);
			MeshComponent->SetMobility(EComponentMobility::Movable);
		}
	}
	if (!OverrideInventoryAutoConfig)
	{
		IsInventoryAddable = DetermineInventoryAddibility();
	}
}

bool UMeshInteractionComponent::DetermineInventoryAddibility() const
{
	if (const AStaticMeshActor* MeshActor {Cast<AStaticMeshActor>(GetOwner())})
	{
		if (const UStaticMeshComponent* MeshComponent {MeshActor->GetStaticMeshComponent()})
		{
			const float Mass {MeshComponent->GetMass()};
			if (Mass > 10.0f) { return false; }

			const FBoxSphereBounds Bounds {MeshComponent->GetStaticMesh()->GetBounds()};
			const FVector BoxExtent {Bounds.BoxExtent};
			const float Volume {static_cast<float>(BoxExtent.X * BoxExtent.Y * BoxExtent.Z * 8.0f)};

			if (Volume > 1000000.0f) { return false; }
			return true;
		}
	}
	return false;
}

bool UMeshInteractionComponent::AddToInventory_Implementation(const AActor* Actor)
{
	if (!Actor) { return false;}
	if (AStaticMeshActor* MeshActor {Cast<AStaticMeshActor>(GetOwner())})
	{
		if (UStaticMeshComponent* MeshComponent {MeshActor->GetStaticMeshComponent()})
		{
			MeshComponent->SetSimulatePhysics(false);
			MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			MeshComponent->SetVisibility(false);
			
			const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
			MeshActor->AttachToActor(const_cast<AActor*>(Actor), AttachmentRules);
		}
	}
	return true;
}

bool UMeshInteractionComponent::TakeFromInventory_Implementation(const AActor* Actor)
{
	if (!Actor) { return false;}
	if (AStaticMeshActor* MeshActor {Cast<AStaticMeshActor>(GetOwner())})
	{
		if (UStaticMeshComponent* MeshComponent {MeshActor->GetStaticMeshComponent()})
		{
			const FDetachmentTransformRules DetachmentRules(EDetachmentRule::KeepWorld, true);
			MeshActor->DetachFromActor(DetachmentRules);

			MeshActor->SetActorLocation(Actor->GetActorForwardVector() * 80);

			MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			MeshComponent->SetVisibility(true);
			MeshComponent->SetSimulatePhysics(true);
		}
	}
	return true;
}
