// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHPlayerDragComponent.h"

#include "BHDraggableObjectInterface.h"
#include "DrawDebugHelpers.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "PhysicsProxy/SingleParticlePhysicsProxy.h"

DEFINE_LOG_CATEGORY_STATIC(LogBHPlayerDragComponent, Log, All)

UBHPlayerDragComponent::UBHPlayerDragComponent()
{
}

void UBHPlayerDragComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UBHPlayerDragComponent::Drag(UObject* Object)
{
	if(!ensureAlwaysMsgf(!IsHoldingObject(), TEXT("Drag: An object is already being dragged.")))
	{
		return;
	}
	if (!IsValid(Object))
	{
		UE_LOG(LogBHPlayerDragComponent, Warning, TEXT("Drag: No valid object provided."))
		return;
	}
	if (!Object->Implements<UBHDraggableObject>())
	{
		UE_LOG(LogBHPlayerDragComponent, Warning, TEXT("Drag: Object [%s] does not implement IBHDraggableObject."),
		*Object->GetName());
		return;
	}
	
	UPrimitiveComponent* DraggableComponent = nullptr;
	if (UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(Object);
		PrimitiveComponent->IsSimulatingPhysics())
	{
		DraggableComponent = PrimitiveComponent;
	}

	// If we lack a draggable component at this point, due to the object possibly being an Actor or Actor Component,
	// we'll seek the first draggable Primitive Component within the Actor.
	if (!DraggableComponent)
	{
		const AActor* Actor = Cast<AActor>(Object);
		if (!Actor)
		{
			if (const UActorComponent* Component = Cast<UActorComponent>(Object))
			{
				Actor = Component->GetOwner();
			}
		}
		if (Actor)
		{
			if (bAllowRootOnly)
			{
				if (UPrimitiveComponent* Root = Cast<UPrimitiveComponent>(Actor->GetRootComponent());
					Root->IsSimulatingPhysics())
				{
					DraggableComponent = Root;
				}
			}
			else
			{
				TArray<UPrimitiveComponent*> Components;
				Actor->GetComponents<UPrimitiveComponent>(Components);
				DraggableComponent = *Algo::FindByPredicate(Components,
					[](const UPrimitiveComponent* Component)
					{
						return IsValid(Component) && Component->IsSimulatingPhysics();
					}
				);
			}
		}
	}

	if (!DraggableComponent)
	{
		UE_LOG(LogBHPlayerDragComponent, Warning, TEXT("Drag: Object [%s] is not a draggable."),
		*Object->GetName());
		return;
	}
	
	// Drag the object if the object lets us.
	// Calling this commits us; we can't abort after due to unknown object behavior.
	const bool bGrabbed = IBHDraggableObject::Execute_Drag(Object, GetOwner());
	if (!bGrabbed)
	{
		UE_LOG(LogBHPlayerDragComponent, Verbose, TEXT("Drag: Object [%s] returned false for IBHDraggableObject::Drag."),
			*Object->GetName());
		return;
	}
	return;
}

void UBHPlayerDragComponent::Release()
{
	return;
}

bool UBHPlayerDragComponent::IsHoldingObject() const
{
	return false;
}

// TODO: Implement after refactor.
// void UBHPlayerDragComponent::UpdateCameraRotationSpeed(float DeltaTime)
// {
// 	if(GrabbedComponent)
// 	{
// 		CameraRotationMultiplier = 1 / (FVector::Distance(GrabbedComponent->GetComponentLocation(), TargetLocation)
// 			* CameraRotationDamping * 0.1);
// 	}
// 	else
// 	{
// 		CameraRotationMultiplier = 1.0;
// 	}
// }
//
// void UBHPlayerDragComponent::UpdateLocalConstraint()
// {
// 	UBHPlayerInteractionComponent* InteractionComponent = GetPlayerCharacter()->GetInteractionComponent();
// 	
// 	if (!HitResult.IsValidBlockingHit() || !(HitResult.GetComponent() == GrabbedComponent)) { return; }
// 	
// 	if (FVector::DistSquared(HitResult.ImpactPoint, GetDragLocation()) >= 1225)
// 	{
// 		/** TODO: Find method to prevent physics jump when re-grabbing object. */
// 		InterpolationSpeed = 0.0f;
// 		GrabComponentAtLocation(HitResult.GetComponent(), NAME_None, HitResult.ImpactPoint);
// 		InterpolationSpeed = 50.0f;
// 	}
// }
//
// void UBHPlayerDragComponent::DragActorAtLocation(AActor* ActorToGrab, const FVector& Location)
// {
// 	if (!ActorToGrab)
// 	{
// 		return;
// 	}
// 	if (GrabbedComponent)
// 	{
// 		return;
// 	}
// 	UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(ActorToGrab->GetComponentByClass(UStaticMeshComponent::StaticClass()));
// 	if (!StaticMeshComponent)
// 	{
// 		return;
// 	}
// 	
// 	UCameraComponent* Camera = GetPlayerCharacter()->GetCamera();
//
// 	TargetLocationZ = Location.Z;
// 	GrabComponentAtLocation(StaticMeshComponent, NAME_None, Location);
// 	CurrentZoomLevel = FVector::Distance(Camera->GetComponentLocation(), Location);
// 	
// 	GrabOffset = StaticMeshComponent->GetCenterOfMass() - (Camera->GetComponentLocation() + CurrentZoomLevel * Camera->GetForwardVector());
//
// 	const FBox BoundingBox = GrabbedComponent->Bounds.GetBox();
// 	DraggedComponentSize = FVector::Distance(BoundingBox.Min, BoundingBox.Max) / 2;
// 	
// 	SetComponentTickEnabled(true);
// }
//
// void UBHPlayerDragComponent::ReleaseActor()
// {
// 	if (!GrabbedComponent){ return; }
//
// 	//GrabbedComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
// 	SetComponentTickEnabled(false);
// 	
// 	ReleaseComponent();
// }
//
// void UBHPlayerDragComponent::UpdateTargetLocation(float DeltaTime)
// {
// 	if (!GrabbedComponent)
// 	{
// 		return;
// 	}
//
// 	CurrentZoomLevel = FMath::Clamp(CurrentZoomLevel, MinZoomLevel, MaxZoomLevel);
//
// 	UCameraComponent* Camera = GetPlayerCharacter()->GetCamera();
//
// 	FVector TargetLocationPre = Camera->GetComponentLocation() + CurrentZoomLevel * Camera->GetForwardVector() ;
// 	TargetLocation = FVector(TargetLocationPre.X,TargetLocationPre.Y, TargetLocationZ);
// 	SetTargetLocation(TargetLocation );
// }
//
// FVector UBHPlayerDragComponent::GetDragLocation() const
// {
// 	if (!GrabbedComponent) { return FVector(); }
// 	
// 	const FTransform ComponentTransform = GrabbedComponent->GetComponentTransform();
// 	
// 	return ComponentTransform.TransformPosition(ConstraintLocalPosition);
// }
//
// void UBHPlayerDragComponent::UpdatePhysicsHandle()
// {
// 	SetLinearDamping(LinearDamping);
// 	SetLinearStiffness(LinearStiffness);
// 	SetAngularDamping(AngularDamping);
// 	SetAngularStiffness(AngularStiffness);
// 	SetInterpolationSpeed(InterpolationSpeed);
// }


