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
		UpdateTargetLocationWithRotation(CurrentZoomAxisValue * DeltaTime);
	}
}


void UPlayerPhysicsGrabComponent::GrabObject(AActor* ObjectToGrab)
{
	// check if there's a reference and cast to static mesh component to get a ref to the first static mesh.
	if (!ObjectToGrab){return;}
	if(GrabbedComponent){return;}
	UStaticMeshComponent* StaticMeshComponent = nullptr;
	StaticMeshComponent = Cast<UStaticMeshComponent>(ObjectToGrab->GetComponentByClass(UStaticMeshComponent::StaticClass()));
	if (StaticMeshComponent)
	{
		CurrentZoomLevel = FVector::Distance(Camera->GetComponentLocation(), StaticMeshComponent->GetCenterOfMass());
		GrabComponentAtLocationWithRotation(StaticMeshComponent, NAME_None,StaticMeshComponent->GetCenterOfMass(),StaticMeshComponent->GetComponentRotation());

		/** Get the original rotation of the grabbed component */
		 OriginalRotation = StaticMeshComponent->GetComponentRotation() - Camera->GetComponentRotation();




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
	const FRotator CameraRotation = Camera->GetComponentRotation();

	// Rotate the hand offset vector using the camera's world rotation
	const FVector RotatedHandOffset = CameraRotation.RotateVector(Configuration->RelativeHoldingHandLocation);

	float HandOffsetScaler = FMath::Clamp((((Configuration->BeginHandOffsetDistance) - CurrentZoomLevel) / Configuration->BeginHandOffsetDistance), 0.0, 1000.0);
	const FVector RotatedHandOffsetMultiplied =  Camera->GetComponentLocation() + HandOffsetScaler * RotatedHandOffset;
	return RotatedHandOffsetMultiplied;
}

void UPlayerPhysicsGrabComponent::UpdateTargetLocationWithRotation(float ZoomAxisValue)
{
	if (!GrabbedComponent) return;
	AActor* CompOwner = this->GetOwner();
	if (CompOwner)
	{
		CurrentZoomLevel = FMath::Clamp(CurrentZoomLevel - Configuration->WalkingRetunZoomSpeed * CompOwner->GetVelocity().Size() + ZoomAxisValue * Configuration->ZoomSpeed, Configuration->MinZoomLevel, Configuration->MaxZoomLevel);
		// Calculate the desired location based on the forward vector and zoom level
	}

	if (Camera)
	{
		const FVector TargetLocation = GetRotatedHandOffset() + (CurrentZoomLevel * Camera->GetForwardVector());

		/** Calculate the difference between the camera rotation and the original rotation */
		RotationDifference = OriginalRotation + Camera->GetComponentRotation();
		
		// Update the rotation of the grabbed component based on the camera rotation
		FRotator TargetRotation = FRotator(0.0f, RotationDifference.Yaw, RotationDifference.Roll);;

		SetTargetLocationAndRotation(TargetLocation,TargetRotation);
		//SetTargetLocation(TargetLocation);


	}
}

void UPlayerPhysicsGrabComponent::UpdateZoomAxisValue(float ZoomAxis)
{
	CurrentZoomAxisValue = FMath::Clamp(((CurrentZoomAxisValue + 0.1 * ZoomAxis) * 0.9),-2.0,2.0);
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

