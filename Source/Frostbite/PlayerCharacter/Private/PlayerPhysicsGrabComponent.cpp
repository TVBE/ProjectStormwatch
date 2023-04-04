// Copyright (c) 2022-present Barrelhouse
// Written by Nino Saglia
// This source code is part of the project Frostbite

#include "PlayerPhysicsGrabComponent.h"

#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/MovementComponent.h"


void UPlayerPhysicsGrabComponent::OnRegister()
{
	Super::OnRegister();

	if (const APlayerCharacter* PlayerCharacter {Cast<APlayerCharacter>(GetOwner())})
	{
		Camera = PlayerCharacter->GetCamera();
		Movement = PlayerCharacter->GetPlayerCharacterMovement();
	}
}

void UPlayerPhysicsGrabComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if(GrabbedComponent)
	{
		UpdateTargetLocationWithRotation(DeltaTime);
	}
}

/** Grab the object pass it to the physicshandle and capture the relative object rotation*/
void UPlayerPhysicsGrabComponent::GrabObject(AActor* ObjectToGrab)
{
	/** check if there's a reference and cast to static mesh component to get a ref to the first static mesh. */
	if (!ObjectToGrab || GrabbedComponent) { return; }
	if (UStaticMeshComponent* StaticMeshComponent {Cast<UStaticMeshComponent>(ObjectToGrab->GetComponentByClass(UStaticMeshComponent::StaticClass()))})
	{
		CurrentZoomLevel = FVector::Distance(Camera->GetComponentLocation(), StaticMeshComponent->GetCenterOfMass());
		GrabComponentAtLocationWithRotation(StaticMeshComponent, NAME_None,StaticMeshComponent->GetCenterOfMass(),StaticMeshComponent->GetComponentRotation());

		/** Get the original rotation of the grabbed component */
		 OriginalRotation = StaticMeshComponent->GetComponentRotation() - Camera->GetComponentRotation();
		/** start the tick function so that the update for the target location can start updating*/
		SetComponentTickEnabled(true);

		StaticMeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	}
}

void UPlayerPhysicsGrabComponent::ReleaseObject()
{
	if(GrabbedComponent)
	{
		GrabbedComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	}
	SetComponentTickEnabled(false);
	ReleaseComponent();
}

void UPlayerPhysicsGrabComponent::UpdateRotatedHandOffset(FRotator& Rotation, FVector& HandOffset)
{
	/** Get the camera's world rotation. */
	CameraRotation = Camera->GetComponentRotation();

	/** Rotate the hand offset vector using the camera's world rotation. */
	RotatedHandOffset =CameraRotation.RotateVector(Configuration->RelativeHoldingHandLocation);

	const float HandOffsetScalar {static_cast<float>(FMath::Clamp((((Configuration->BeginHandOffsetDistance)
		- CurrentZoomLevel) / Configuration->BeginHandOffsetDistance), 0.0, 1000.0))};
	
	RotatedHandOffset = Camera->GetComponentLocation() + HandOffsetScalar * RotatedHandOffset;
}

/** The looping function that updates the target location and rotation of the currently grabbed object*/
void UPlayerPhysicsGrabComponent::UpdateTargetLocationWithRotation(float DeltaTime)
{
	if (!GrabbedComponent) return;
	AActor* CompOwner = this->GetOwner();
	if (CompOwner)
	{
		/** Update the zoom level dependent on the scroll wheel and the movement type.*/
		if(Movement && Movement->GetIsSprinting())
		{
			CurrentZoomLevel = CurrentZoomLevel - Configuration->WalkingRetunZoomSpeed * DeltaTime;
		}
		else
		{
			CurrentZoomLevel = CurrentZoomLevel + CurrentZoomAxisValue * Configuration->ZoomSpeed * DeltaTime;
		}
		/** In any case, clam the zoom level within the min max of configuration*/
		CurrentZoomLevel = FMath::Clamp(CurrentZoomLevel, Configuration->MinZoomLevel, Configuration->MaxZoomLevel);
		
	}

	if (Camera)
	{
		UpdateRotatedHandOffset(CameraRotation, RotatedHandOffset);
		const FVector TargetLocation = RotatedHandOffset + (CurrentZoomLevel * Camera->GetForwardVector());

		/** Calculate the difference between the camera rotation and the original rotation */
		RotationDifference = OriginalRotation + Camera->GetComponentRotation();
		
		/** Update the rotation of the grabbed component based on the camera rotation */
		FRotator TargetRotation = FRotator(0.0f, RotationDifference.Yaw, RotationDifference.Roll);;

		SetTargetLocationAndRotation(TargetLocation,TargetRotation);



	}
}
/** The update loop that scales the zoomaxis value from the mouse input */
void UPlayerPhysicsGrabComponent::UpdateZoomAxisValue(float ZoomAxis)
{
	CurrentZoomAxisValue = FMath::Clamp(((CurrentZoomAxisValue + 0.1 * ZoomAxis) * 0.9),-2.0,2.0);
}

void UPlayerPhysicsGrabConfiguration::ApplyToPhysicsHandle(UPhysicsHandleComponent* PhysicsHandleComponent)
{
	// Set the member variables of this PhysicsHandleComponent to the values in this data asset.
	this->bSoftAngularConstraint = bSoftAngularConstraint;
	this->bSoftLinearConstraint = bSoftLinearConstraint;
	this->bInterpolateTarget = bInterpolateTarget;
	this->LinearDamping = LinearDamping;
	this->LinearStiffness = LinearStiffness;
	this->AngularDamping = AngularDamping;
	this->AngularStiffness = AngularStiffness;
	this->InterpolationSpeed = InterpolationSpeed;
}

