// Copyright (c) 2022-present Barrelhouse
// Written by Nino Saglia
// This source code is part of the project Frostbite

#include "PlayerDragComponent.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "PlayerCharacter.h"


DEFINE_LOG_CATEGORY_CLASS(UPlayerDragComponent, LogDragComponent)

UPlayerDragComponent::UPlayerDragComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}



void UPlayerDragComponent::OnRegister()
{
    Super::OnRegister();

    if (const APlayerCharacter* PlayerCharacter{ Cast<APlayerCharacter>(GetOwner()) })
    {
        Camera = PlayerCharacter->GetCamera();
        Movement = PlayerCharacter->GetPlayerCharacterMovement();
    }
    else
    {
        UE_LOG(LogDragComponent, Warning, TEXT("Player character is not valid."));
    }

    UWorld* World = GetWorld();
    if (World != nullptr)
    {
        Gravity = World->GetGravityZ();
    }
    else{UE_LOG(LogDragComponent, Warning, TEXT("World is not valid."));}

	if(!Configuration)
	{
		UE_LOG(LogDragComponent, Warning, TEXT("Created Default configuration since it's not set."));
		Configuration = NewObject<UDragConfiguration>();
	}
}

void UPlayerDragComponent::BeginPlay()
{
    Super::BeginPlay();
	SetComponentTickEnabled(false);
	
}

void UPlayerDragComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    if (DraggedComponent)
    {
    	/** Some nullpointer checks.*/
        if (!Configuration){UE_LOG(LogDragComponent, Warning, TEXT("Configuration is not valid."));return;}

    	UpdateTargetForceWithLocation(DeltaTime);
        if (Configuration->LetGoDistance <= FVector::Distance(DraggedComponent->GetComponentLocation(), TargetLocation))
        {
            ReleaseActor();
        }
    }
    else{UE_LOG(LogDragComponent, Warning, TEXT("Dragged component is not valid."));}
}

void UPlayerDragComponent::DragActorAtLocation(AActor* ActorToGrab, const FVector& Location)
{
	if (!ActorToGrab){UE_LOG(LogDragComponent, Warning, TEXT("Actor to grab is null"));return;}
	if (DraggedComponent){UE_LOG(LogDragComponent, Warning, TEXT("Already dragging a component"));return;}

	UStaticMeshComponent* StaticMeshComponent {Cast<UStaticMeshComponent>(ActorToGrab->GetComponentByClass(UStaticMeshComponent::StaticClass()))};
	if (!StaticMeshComponent){UE_LOG(LogDragComponent, Warning, TEXT("Actor to grab does not have a static mesh component"));return;}

	CurrentZoomLevel = FVector::Distance(Camera->GetComponentLocation(), StaticMeshComponent->GetCenterOfMass());

	DraggedComponent = StaticMeshComponent;
	DraggedLocationOffset = DraggedComponent->GetComponentTransform().InverseTransformPosition(Location);

	SetComponentTickEnabled(true);
	DraggedComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	FBox BoundingBox {DraggedComponent->Bounds.GetBox()};
	DraggedComponentSize = FVector::Distance(BoundingBox.Min, BoundingBox.Max) / 2;
}

void UPlayerDragComponent::ReleaseActor()
{
	if (!DraggedComponent){UE_LOG(LogDragComponent, Warning, TEXT("No component is currently being dragged"));return;}

	DraggedComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	SetComponentTickEnabled(false);
	DraggedComponent = nullptr;

	UE_LOG(LogDragComponent, VeryVerbose, TEXT("Released Object."));
}

/** The looping function that updates the target location and rotation of the currently dragged object*/
void UPlayerDragComponent::UpdateTargetForceWithLocation(float DeltaTime)
{
	if (!DraggedComponent || !Camera || !Configuration) { UE_LOG(LogDragComponent, Warning, TEXT("UpdateTargetForceWithLocation called with null pointers"));return; }
	AActor* CompOwner = this->GetOwner();

	if (CompOwner)
	{
		{
			CurrentZoomLevel = CurrentZoomLevel + CurrentZoomAxisValue * Configuration->ZoomSpeed * DeltaTime;
		}

		/** Clamp the zoom level within the min max of configuration*/
		CurrentZoomLevel = FMath::Clamp(CurrentZoomLevel, Configuration->MinZoomLevel, Configuration->MaxZoomLevel);
	}

	TargetLocation = CurrentZoomLevel * Camera->GetForwardVector();

	ApplyTargetForce(TargetLocation, ApplyForceOnCenterMass);
}

void UPlayerDragComponent::ApplyTargetForce(FVector Location, bool OnCenterMass)
{
	if (!DraggedComponent) { UE_LOG(LogDragComponent, Warning, TEXT("ApplyTargetForce called with null DraggedComponent"));return;}
	FVector ForceLocation;
	FVector Force{ 100*(Location - (DraggedComponent->GetCenterOfMass() + DraggedLocationOffset))};
	//TODO: This calculation is completely messed up, but at least it's working, i'll figure it out another time.
	if (OnCenterMass)
	{
		ForceLocation = DraggedComponent->GetCenterOfMass(NAME_None);
		DraggedComponent->AddForceAtLocation(Force, ForceLocation, NAME_None);
	}
	else
	{
		ForceLocation = DraggedLocationOffset;
		DraggedComponent->AddForceAtLocationLocal(Force, ForceLocation, NAME_None);
		UE_LOG(LogTemp,Warning,TEXT("Addforceatlocation"))
	}
}

/** The update loop that scales the zoomaxis value from the mouse input */
void UPlayerDragComponent::UpdateZoomAxisValue(float ZoomAxis)
{
	if (!Configuration){UE_LOG(LogDragComponent, Warning, TEXT("UpdateZoomAxisValue called with null Configuration"));return;}
	CurrentZoomAxisValue = FMath::Clamp(((CurrentZoomAxisValue + 0.1 * ZoomAxis) * 0.9), -2.0, 2.0);
}