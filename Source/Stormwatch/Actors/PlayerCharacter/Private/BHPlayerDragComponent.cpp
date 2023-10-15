// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHPlayerDragComponent.h"
#include "BHPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "BHPlayerInteractionComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "PhysicsProxy/SingleParticlePhysicsProxy.h"

DEFINE_LOG_CATEGORY_CLASS(UBHPlayerDragComponent, LogDragComponent)

void UBHPlayerDragComponent::OnRegister()
{
    Super::OnRegister();
	
	UpdatePhysicsHandle();
}

void UBHPlayerDragComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UBHPlayerDragComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    if (GrabbedComponent)
    {    	
    	UpdateTargetLocation(DeltaTime);

        if (AutoReleaseDistance <= FVector::Distance(GrabbedComponent->GetComponentLocation(), TargetLocation))
        {
            ReleaseActor();
        }
    }
}

void UBHPlayerDragComponent::UpdateCameraRotationSpeed(float DeltaTime)
{
	if(GrabbedComponent)
	{
		CameraRotationMultiplier = 1 / (FVector::Distance(GrabbedComponent->GetComponentLocation(), TargetLocation)
			* CameraRotationDamping * 0.1);
	}
	else
	{
		CameraRotationMultiplier = 1.0;
	}
}

void UBHPlayerDragComponent::UpdateLocalConstraint()
{
	UBHPlayerInteractionComponent* InteractionComponent = GetPlayerCharacter()->GetInteractionComponent();

	const FHitResult HitResult = InteractionComponent->GetCameraTraceHitResult();
	if (!HitResult.IsValidBlockingHit() || !(HitResult.GetComponent() == GrabbedComponent)) { return; }
	
	if (FVector::DistSquared(HitResult.ImpactPoint, GetDragLocation()) >= 1225)
	{
		/** TODO: Find method to prevent physics jump when re-grabbing object. */
		InterpolationSpeed = 0.0f;
		GrabComponentAtLocation(HitResult.GetComponent(), NAME_None, HitResult.ImpactPoint);
		InterpolationSpeed = 50.0f;
	}
}

void UBHPlayerDragComponent::DragActorAtLocation(AActor* ActorToGrab, const FVector& Location)
{
	if (!ActorToGrab)
	{
		return;
	}
	if (GrabbedComponent)
	{
		return;
	}
	UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(ActorToGrab->GetComponentByClass(UStaticMeshComponent::StaticClass()));
	if (!StaticMeshComponent)
	{
		return;
	}
	
	UCameraComponent* Camera = GetPlayerCharacter()->GetCamera();

	TargetLocationZ = Location.Z;
	GrabComponentAtLocation(StaticMeshComponent, NAME_None, Location);
	CurrentZoomLevel = FVector::Distance(Camera->GetComponentLocation(), Location);
	
	GrabOffset = StaticMeshComponent->GetCenterOfMass() - (Camera->GetComponentLocation() + CurrentZoomLevel * Camera->GetForwardVector());

	const FBox BoundingBox = GrabbedComponent->Bounds.GetBox();
	DraggedComponentSize = FVector::Distance(BoundingBox.Min, BoundingBox.Max) / 2;
	
	SetComponentTickEnabled(true);
}

void UBHPlayerDragComponent::ReleaseActor()
{
	if (!GrabbedComponent){ return; }

	//GrabbedComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	SetComponentTickEnabled(false);
	
	ReleaseComponent();
}

void UBHPlayerDragComponent::UpdateTargetLocation(float DeltaTime)
{
	if (!GrabbedComponent)
	{
		return;
	}

	CurrentZoomLevel = FMath::Clamp(CurrentZoomLevel, MinZoomLevel, MaxZoomLevel);

	UCameraComponent* Camera = GetPlayerCharacter()->GetCamera();

	FVector TargetLocationPre = Camera->GetComponentLocation() + CurrentZoomLevel * Camera->GetForwardVector() ;
	TargetLocation = FVector(TargetLocationPre.X,TargetLocationPre.Y, TargetLocationZ);
	SetTargetLocation(TargetLocation );
}

FVector UBHPlayerDragComponent::GetDragLocation() const
{
	if (!GrabbedComponent) { return FVector(); }
	
	const FTransform ComponentTransform = GrabbedComponent->GetComponentTransform();
	
	return ComponentTransform.TransformPosition(ConstraintLocalPosition);
}

void UBHPlayerDragComponent::UpdatePhysicsHandle()
{
	SetLinearDamping(LinearDamping);
	SetLinearStiffness(LinearStiffness);
	SetAngularDamping(AngularDamping);
	SetAngularStiffness(AngularStiffness);
	SetInterpolationSpeed(InterpolationSpeed);
}


