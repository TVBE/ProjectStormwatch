// Copyright (c) 2022-present Barrelhouse
// Written by Nino Saglia
// This source code is part of the project Frostbite

#include "PlayerGrabComponent.h"

#include "KineticActorComponent.h"
#include "LogCategories.h"
#include "PlayerCharacter.h"
#include "SWarningOrErrorBox.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/MovementComponent.h"
#include "Serialization/JsonTypes.h"


void UPlayerGrabComponent::OnRegister()
{
	Super::OnRegister();

	if (const APlayerCharacter* PlayerCharacter {Cast<APlayerCharacter>(GetOwner())})
	{
		Camera = PlayerCharacter->GetCamera();
		Movement = PlayerCharacter->GetPlayerCharacterMovement();
		
	}
}

void UPlayerGrabComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (GrabbedComponent)
	{
		UpdateTargetLocationWithRotation(DeltaTime);
		
		/** If the distance between the location and target location is too big, let the object go. */
		//TODO: Release is triggered here immediately the first time the player grabs an object.
		if (!IsPrimingThrow) 
		{
			if (Configuration->LetGoDistance <= FVector::Distance(GrabbedComponent->GetComponentLocation(), TargetLocation))
			{
				ReleaseObject();
			}
		}

		/** The looping function to handle the priming of the throw*/
		if(IsPrimingThrow)
		{
			/** The timer that handles the time it takes before the player starts priming the throw.*/
			if(PrePrimingThrowTimer <= Configuration->PrePrimingThrowDelayTime)
			{
				PrePrimingThrowTimer += DeltaTime;
			}
			else
			{
				if(CurrentZoomLevel != Configuration->ThrowingZoomLevel)
				{
					CurrentZoomLevel += Configuration->ToThrowingZoomSpeed * (Configuration->ThrowingZoomLevel - CurrentZoomLevel);
				}
				WillThrowOnRelease = true;
				if(PrePrimingThrowTimer <= 1.0)
				{
					/**The timeline to be used by various on tick functions that update location.
					 *We normalize the value here since it otherwise would be need to be normalised multiple times later.*/
					ThrowingTimeLine += FMath::Clamp(DeltaTime/Configuration->ThrowChargeTime, 0.0,1.0);
				}
			}
		}
	}
}

/** Grab the object pass it to the physicshandle and capture the relative object rotation*/
void UPlayerGrabComponent::GrabActor(AActor* ActorToGrab)
{
	/** check if there's a reference and cast to static mesh component to get a ref to the first static mesh. */
	if (!ActorToGrab || GrabbedComponent) { return; }
	if (UStaticMeshComponent* StaticMeshComponent {Cast<UStaticMeshComponent>(ActorToGrab->GetComponentByClass(UStaticMeshComponent::StaticClass()))})
	{
		CurrentZoomLevel = FVector::Distance(Camera->GetComponentLocation(), StaticMeshComponent->GetCenterOfMass());
		GrabComponentAtLocationWithRotation(StaticMeshComponent, NAME_None,StaticMeshComponent->GetCenterOfMass(),StaticMeshComponent->GetComponentRotation());

		/** Get the GrabbedComponent rotation relative to the camera. */


		 CameraRelativeRotation = Camera->GetComponentQuat().Inverse() * GrabbedComponent->GetComponentQuat();
	
		/** Start the tick function so that the update for the target location can start updating. */
		SetComponentTickEnabled(true);

		/** Disable the colission with the player. */
		StaticMeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	}


	/** Check if the actor already has a kinetic component. If not, add the component to the grabbed actor. */
	if (const UActorComponent* KineticComponent {ActorToGrab->GetComponentByClass(UKineticActorComponent::StaticClass())}; !KineticComponent)
	{
		ActorToGrab->AddComponentByClass(UKineticActorComponent::StaticClass(), false, FTransform(), false);
	}
	
	FBox BoundingBox {GrabbedComponent->Bounds.GetBox()};
	GrabbedComponentSize = FVector::Distance(BoundingBox.Min, BoundingBox.Max)/2;
}

void UPlayerGrabComponent::ReleaseObject()
{
	if(GrabbedComponent)
	{
		GrabbedComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
		SetComponentTickEnabled(false);
		OnGrabbedObjectReleased.Broadcast(GrabbedComponent->GetOwner());
		ReleaseComponent();
		UE_LOG(LogGrabComponent, VeryVerbose, TEXT("Released Object."))
		StopPrimingThrow();
		WillThrowOnReleaseMultiplier = 1.0;
	}
}


void UPlayerGrabComponent::BeginPrimingThrow()
{
	IsPrimingThrow = true;
	PrePrimingThrowTimer = 0.0;
	ThrowingTimeLine = 0.0f;
	UE_LOG(LogGrabComponent, VeryVerbose, TEXT("Started Priming Throw."))
}

void UPlayerGrabComponent::StopPrimingThrow()
{
	IsPrimingThrow = false;
	WillThrowOnRelease = false;
	PrePrimingThrowTimer = 0.0;
	ThrowingTimeLine = 0.0f;
	UE_LOG(LogGrabComponent, VeryVerbose, TEXT("Stopped Priming Throw."))
}

/** Execute a throw if the throw is priming*/
void UPlayerGrabComponent::PerformThrow()
{
	if(IsPrimingThrow)
	{
		
		FVector Target {FVector()};
		FVector TraceStart {Camera->GetComponentLocation()+RotatedHandOffset};
		FVector TraceEnd {Camera->GetForwardVector() * 100000 + TraceStart};FHitResult HitResult;
		ReleaseLocation = Camera->GetComponentLocation() + 10 * Camera->GetForwardVector() + 10 * Camera->GetUpVector();
		FCollisionQueryParams CollisionParams;
		if  (this->GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, CollisionParams))
			{
				Target = HitResult.ImpactPoint;
			}
			else
			{
				Target = TraceEnd;
			}
		
		/** Calculate the throwing strenght using the timeline we updated in the tick.*/
		const float ThrowingStrength = FMath::Lerp(Configuration->MinThrowingStrength, Configuration->MaxThrowingStrength, FMath::Clamp((ThrowingTimeLine),0.0,1.0));

		float ThrowAngle = CalculateThrowAngle(ReleaseLocation,TargetLocation,ThrowingStrength);
		
		
		/** Calculate the direction from the player to the target */
		FVector Direction = Target - TraceStart;
		Direction.Normalize();
	

		/**Rotate the Direction vector around the cross product of the Direction vector and the world up vector by the angle Theta. */
		FVector RotationAxis = Direction.CrossProduct(Direction,FVector::UpVector);
		FVector FinalDirection = Direction.RotateAngleAxis(ThrowAngle + 45, RotationAxis);
	

		FinalDirection.Normalize();

		/** Set the physics velocity of the grabbed component to the calculated throwing direction */
		GrabbedComponent->SetPhysicsLinearVelocity(FinalDirection * ThrowingStrength);
		
		GrabbedComponent->WakeRigidBody();
		/** Release the grabbed component after the throw */
		GrabbedComponent->SetWorldLocation(ReleaseLocation);
		ReleaseObject();
	}
}


float UPlayerGrabComponent::CalculateThrowAngle(FVector StartLocation, FVector Target, float Velocity)
{
	// Calculate displacement vector
	FVector Displacement = Target - StartLocation;

	// Calculate horizontal distance to target location
	float HorizontalDistance = FVector::DistXY(StartLocation, Target);

	// Calculate initial velocity required to hit the target
	const float Gravity = 980.0f; // acceleration due to gravity
	float Theta = atan((Gravity * HorizontalDistance) / (Velocity * Velocity));
	float InitialVelocity = Velocity / cos(Theta);

	// Solve for the angle theta
	Theta = atan((InitialVelocity * InitialVelocity - sqrt(InitialVelocity * InitialVelocity * InitialVelocity * InitialVelocity - Gravity * (Gravity * HorizontalDistance * HorizontalDistance + 2 * Displacement.Z * InitialVelocity * InitialVelocity))) / (Gravity * HorizontalDistance));

	// Return angle;
	return FMath::RadiansToDegrees(Theta);
}


void UPlayerGrabComponent::UpdateRotatedHandOffset(FRotator& Rotation, FVector& HandOffset)
{
	/** Get the camera's world rotation. */
	CameraRotation = Camera->GetComponentRotation();
	CameraQuat = Camera->GetComponentQuat();

	const FVector MinValues = FVector(-Configuration->ThrowingShakeSize,-Configuration->ThrowingShakeSize,-Configuration->ThrowingShakeSize);
	const FVector MaxValues = FVector(Configuration->ThrowingShakeSize,Configuration->ThrowingShakeSize,Configuration->ThrowingShakeSize);
	const FVector ThrowingShake = FMath::RandPointInBox(FBox(MinValues, MaxValues));
	const FVector ThrowingVector = (ThrowingShake + Configuration->ThrowingBackupVector)*FMath::Clamp((ThrowingTimeLine),0.0,1.0);
	
	/** Rotate the hand offset vector using the camera's world rotation. */
	RotatedHandOffset =CameraRotation.RotateVector(Configuration->RelativeHoldingHandLocation + ThrowingVector);

	const float HandOffsetScalar {static_cast<float>(FMath::Clamp((((Configuration->BeginHandOffsetDistance)
		- CurrentZoomLevel) / (Configuration->BeginHandOffsetDistance + Configuration->BeginHandOffsetDistance * GrabbedComponentSize)), 0.0, 1000.0))};

	
	FVector NormalizedRotatedHandOffset = RotatedHandOffset.GetSafeNormal();


	float OffsetScalar = HandOffsetScalar * GrabbedComponentSize;


	FVector RotatedScaledHandOffset = OffsetScalar * (RotatedHandOffset + NormalizedRotatedHandOffset * GrabbedComponentSize);


	RotatedHandOffset = Camera->GetComponentLocation() + RotatedScaledHandOffset;
	
}

/** The looping function that updates the target location and rotation of the currently grabbed object*/
void UPlayerGrabComponent::UpdateTargetLocationWithRotation(float DeltaTime)
{
	if (!GrabbedComponent) { return; }
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
		TargetLocation = RotatedHandOffset * WillThrowOnReleaseMultiplier + (CurrentZoomLevel * Camera->GetForwardVector());

		
		
		FRotator TargetRotation {CameraQuat * CameraRelativeRotation};
		
		SetTargetLocationAndRotation( TargetLocation,TargetRotation);
	}
}



/** Updates on tick when you are manually rotating the object.*/
void UPlayerGrabComponent::UpdateMouseImputRotation(FVector2d MouseInputDelta)
{
	if(RotationMode)
	{
		/** Make rotations based on mouse movement and scroll wheel in the perspective of the world.*/
		FQuat MouseInputQuatX{FQuat(1,0,0,CurrentRotationZoomAxisValue*0.07)};
		FQuat MouseInPutQuatY{FQuat(0,1,0,-MouseInputDelta.Y*0.01)};
		FQuat MouseInPutQuatZ{FQuat(0,0,1,MouseInputDelta.X*0.01)};
		FQuat DeltaRotation = MouseInputQuatX * MouseInPutQuatY * MouseInPutQuatZ;
		
		/**Normalize this rotation and apply it to the relative rotation of the object.*/
		CameraRelativeRotation = DeltaRotation.GetNormalized() * CameraRelativeRotation;
		
	}
}

void UPlayerGrabComponent::BeginTetriaryInteraction()
{
	RotationMode = true;
}

void UPlayerGrabComponent::EndTetriaryInteraction()
{
	RotationMode = false;
}

/** The update loop that scales the zoomaxis value from the mouse input */
void UPlayerGrabComponent::UpdateZoomAxisValue(float ZoomAxis)
{
	if(!RotationMode)
	{
		CurrentZoomAxisValue = FMath::Clamp(((CurrentZoomAxisValue + 0.1 * ZoomAxis) * 0.9),-2.0,2.0);
	}
	else
	{
		CurrentRotationZoomAxisValue = ZoomAxis; //FMath::Clamp(((CurrentZoomAxisValue + 0.1 * ZoomAxis) * 0.9),-2.0,2.0);
	}
	
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

