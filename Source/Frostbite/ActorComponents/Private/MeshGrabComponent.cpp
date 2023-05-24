// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#include "MeshGrabComponent.h"

#include "Engine/StaticMeshActor.h"

inline bool DetermineInventoryAddibility(const UActorComponent* Component)
{
	if (!Component) { return false; }
	if (const AStaticMeshActor* MeshActor {Cast<AStaticMeshActor>(Component->GetOwner())})
	{
		if (const UStaticMeshComponent* MeshComponent {MeshActor->GetStaticMeshComponent()})
		{
			const float Mass {MeshComponent->GetMass()};
			if (Mass > 10.0f) { return false; }

			const FBoxSphereBounds BoxSphereBounds {MeshComponent->GetStaticMesh()->GetBounds()};
			const FVector BoxExtent {BoxSphereBounds.BoxExtent};
			const float Volume {static_cast<float>(BoxExtent.X * BoxExtent.Y * BoxExtent.Z * 8.0f)};

			if (Volume > 1000000.0f) { return false; }
			return true;
		}
	}
	return false;
}

bool UMeshGrabComponent::AddToInventory_Implementation(const AActor* Actor)
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

bool UMeshGrabComponent::TakeFromInventory_Implementation(const AActor* Actor)
{
	if (!Actor) { return false;}

	UStaticMeshComponent* MeshComponent;
	
	if (const AStaticMeshActor* MeshActor {Cast<AStaticMeshActor>(GetOwner())})
	{
		MeshComponent = MeshActor->GetStaticMeshComponent();
	}
	else
	{
		MeshComponent = Cast<UStaticMeshComponent>(GetOwner()->FindComponentByClass(UStaticMeshComponent::StaticClass()));
	}

	if(MeshComponent)
	{
		const FDetachmentTransformRules DetachmentRules(EDetachmentRule::KeepWorld, true);
		GetOwner()->DetachFromActor(DetachmentRules);

		GetOwner()->SetActorLocation(Actor->GetActorForwardVector() * 80);

		MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		MeshComponent->SetVisibility(true);
		MeshComponent->SetSimulatePhysics(true);
	}
	
	return true;
}

void UMeshGrabComponent::OnRegister()
{
	Super::OnRegister();

	//if (!OverrideInventoryAutoConfig)
	//{
	//	IsInventoryAddable = DetermineInventoryAddibility(this);
	//}
}

