// Copyright (c) 2022-present Barrelhouse
// Written by Nino Saglia
// This source code is part of the project Frostbite

#include "PlayerDragComponent.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"


DEFINE_LOG_CATEGORY_CLASS(UPlayerDragComponent, LogDragComponent)

UPlayerDragComponent::UPlayerDragComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerDragComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPlayerDragComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (DraggedComponent)
	{
		UpdateTargetForceWithLocation(DeltaTime);
		if (Configuration->LetGoDistance <= FVector::Distance(DraggedComponent->GetComponentLocation(), TargetLocation))
		{
			ReleaseActor();
		}
	}
}

void UPlayerDragComponent::DragActorAtLocation(AActor* ActorToGrab, FVector Location)
{
	/** check if there's a reference and cast to static mesh component to get a ref to the first static mesh. */
	if (!ActorToGrab || DraggedComponent) { return; }
	if (UStaticMeshComponent* StaticMeshComponent {Cast<UStaticMeshComponent>(ActorToGrab->GetComponentByClass(UStaticMeshComponent::StaticClass()))})
	{
		CurrentZoomLevel = FVector::Distance(Camera->GetComponentLocation(), StaticMeshComponent->GetCenterOfMass());

		DraggedLocationOffset = DraggedComponent->GetComponentTransform().InverseTransformPosition(Location);
		/** Start the tick function so that the update for the target location can start updating. */
		SetComponentTickEnabled(true);
		
		/** Disable the colission with the player. */
		StaticMeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	}
	
	FBox BoundingBox {DraggedComponent->Bounds.GetBox()};
	DraggedComponentSize = FVector::Distance(BoundingBox.Min, BoundingBox.Max)/2;
	
}

void UPlayerDragComponent::ReleaseActor()
{
	if(DraggedComponent)
	{
		DraggedComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
		SetComponentTickEnabled(false);

		UE_LOG(LogDragComponent, VeryVerbose, TEXT("Released Object."))
	}
}

/** The looping function that updates the target location and rotation of the currently dragged object*/
void UPlayerDragComponent::UpdateTargetForceWithLocation(float DeltaTime)
{
	if (!DraggedComponent) { return; }
	AActor* CompOwner = this->GetOwner();
	
	if (CompOwner)
	{
		{
			CurrentZoomLevel = CurrentZoomLevel + CurrentZoomAxisValue * Configuration->ZoomSpeed * DeltaTime;
		}
		
		/** Clamp the zoom level within the min max of configuration*/
		CurrentZoomLevel = FMath::Clamp(CurrentZoomLevel, Configuration->MinZoomLevel, Configuration->MaxZoomLevel);
	}
	
	if (Camera)
	{
		TargetLocation = CurrentZoomLevel * Camera->GetForwardVector();
		
		ApplyTargetForce(TargetLocation,ApplyForceOnCenterMass);
	}
}

void UPlayerDragComponent::ApplyTargetForce(FVector Location, bool OnCenterMass)
{
	FVector ForceLocation;
	FVector Force{Location - (DraggedComponent->GetCenterOfMass()+DraggedLocationOffset)};
	if(OnCenterMass)
	{
		ForceLocation = DraggedComponent->GetCenterOfMass(NAME_None);
		DraggedComponent->AddForceAtLocation(Force,ForceLocation, NAME_None);
	}
	else
	{
		ForceLocation = DraggedLocationOffset;
		DraggedComponent->AddForceAtLocationLocal(Force,ForceLocation, NAME_None);
	}
}

/** The update loop that scales the zoomaxis value from the mouse input */
void UPlayerDragComponent::UpdateZoomAxisValue(float ZoomAxis)
{
	CurrentZoomAxisValue = FMath::Clamp(((CurrentZoomAxisValue + 0.1 * ZoomAxis) * 0.9),-2.0,2.0);
}