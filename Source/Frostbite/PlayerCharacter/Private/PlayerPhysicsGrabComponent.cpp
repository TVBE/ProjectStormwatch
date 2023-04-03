// Copyright (c) 2022-present Barrelhouse
// Written by Nino Saglia
// This source code is part of the project Frostbite

#include "PlayerPhysicsGrabComponent.h"

#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"


void UPlayerPhysicsGrabComponent::OnRegister()
{
	Super::OnRegister();

	if (const APlayerCharacter* PlayerCharacter {Cast<APlayerCharacter>(GetOwner())})
	{
		Camera = PlayerCharacter->GetCamera();
	}
}

void UPlayerPhysicsGrabComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if(GrabbedComponent)
	{
		UpdateTargetLocation(CurrentZoomAxisValue * DeltaTime);
	}
}

void UPlayerPhysicsGrabComponent::GrabObject(AActor* ObjectToGrab)
{
	// check if there's a reference and cast to static mesh component to get a ref to the first static mesh.
	if (!ObjectToGrab){return;}
	UStaticMeshComponent* StaticMeshComponent = nullptr;
	StaticMeshComponent = Cast<UStaticMeshComponent>(ObjectToGrab->GetComponentByClass(UStaticMeshComponent::StaticClass()));
	if (StaticMeshComponent)
	{
		CurrentZoomLevel = FVector::Distance(Camera->GetComponentLocation(), StaticMeshComponent->GetCenterOfMass());
		GrabComponentAtLocation(StaticMeshComponent, NAME_None,StaticMeshComponent->GetCenterOfMass());
		bIsTickEnabled = true;
	}
}

void UPlayerPhysicsGrabComponent::ReleaseObject()
{
	ReleaseComponent();
	bIsTickEnabled = false;
	
}

FVector UPlayerPhysicsGrabComponent::GetRotatedHandOffset()
{
	// Get the camera's world rotation
	FRotator CameraRotation = Camera->GetComponentRotation();

	// Rotate the hand offset vector using the camera's world rotation
	FVector RotatedHandOffset = Camera->GetComponentLocation() + CameraRotation.RotateVector(Configuration->RelativeHoldingHandLocation);

	return RotatedHandOffset;
}

void UPlayerPhysicsGrabComponent::UpdateTargetLocation(float ZoomAxisValue)
{
	if (!GrabbedComponent) return;
	CurrentZoomLevel = FMath::Clamp(CurrentZoomLevel + ZoomAxisValue * Configuration->ZoomSpeed, Configuration->MinZoomLevel, Configuration->MaxZoomLevel);
	UE_LOG(LogTemp, Log, TEXT("ZoomAxisvalue %f"), ZoomAxisValue);
	// Calculate the desired location based on the forward vector and zoom level
	if (Camera)
	{
		const FVector TargetLocation = Camera->GetComponentLocation() + (CurrentZoomLevel * Camera->GetForwardVector());
		SetTargetLocation(TargetLocation);
	}
}

void UPlayerPhysicsGrabComponent::UpdateZoomAxisValue(float ZoomAxis)
{
	CurrentZoomAxisValue = ZoomAxis;
}

void UPlayerPhysicsGrabConfiguration::ApplyToPhysicsHandle(UPhysicsHandleComponent* PhysicsHandleComponent)
{
	// Set the member variables of this PhysicsHandleComponent to the values in this data asset.
	this->LinearDamping = LinearDamping;
	this->AngularDamping = AngularDamping;
	this->InterpolationSpeed = InterpolationSpeed;
	this->GrabDistance = GrabDistance;
	this->bSoftAngularConstraint = bSoftAngularConstraint;
	this->InterpolationSpeedThreshold = InterpolationSpeedThreshold;
}

