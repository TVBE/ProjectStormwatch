// Copyright (c) 2022-present Barrelhouse
// Written by Nino Saglia
// This source code is part of the project Frostbite

#include "PlayerPhysicsGrabComponent.h"

#include "PlayerCharacter.h"
#include "SWarningOrErrorBox.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/MovementComponent.h"
#include "Serialization/JsonTypes.h"


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
		/** If the distance between the location and target location is too big, let the object go.*/
		if(!IsPrimingThrow)
		{
			if(Configuration->LetGoDistance <= FVector::Distance(GrabbedComponent->GetComponentLocation(), TargetLocation))
			{
				OnPhysicsGrabComponentReleased.Broadcast(GrabbedComponent->GetOwner());
				ReleaseObject();
			}
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

	{
		
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
		 OriginalRotation = StaticMeshComponent->GetRelativeRotation();
	
		/** start the tick function so that the update for the target location can start updating*/
		SetComponentTickEnabled(true);

		/** Dissable the colission with the player*/
		StaticMeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		
		/** Enable continous collision detection to prevent the player from being able to clip objects through walls. */
		GrabbedComponent->SetUseCCD(true);
	}
	/**Reset the mouse rotation when you grab a new object*/
	MouseInputRotation = FRotator{0.0,0.0,0.0};


	FBox BoundingBox = GrabbedComponent->Bounds.GetBox();
	GrabbedComponentSize = FVector::Distance(BoundingBox.Min, BoundingBox.Max)/2;
}

void UPlayerPhysicsGrabComponent::ReleaseObject()
{
	if(GrabbedComponent)
	{
		GrabbedComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
		SetComponentTickEnabled(false);
		GrabbedComponent->SetUseCCD(false);
		if(WillThrowOnRelease)
		{
			GrabbedComponent->SetWorldLocation(Camera->GetComponentLocation() - 10 * Camera->GetForwardVector() + 10 * Camera->GetUpVector());
		}
		ReleaseComponent();
		UE_LOG(LogTemp,Warning, TEXT("ReleaseObject"))
		StopPrimingThrow();
		WillThrowOnReleaseMultiplier = 1.0;
	}
}


void UPlayerPhysicsGrabComponent::PrimeThrow()
{
	IsPrimingThrow = true;
	PrePrimingThrowTimer = 0.0;
	ThrowingTimeLine = 0.0f;
	UE_LOG(LogTemp,Warning, TEXT("PrimeThrow"))
}

void UPlayerPhysicsGrabComponent::StopPrimingThrow()
{
	IsPrimingThrow = false;
	WillThrowOnRelease = false;
	PrePrimingThrowTimer = 0.0;
	ThrowingTimeLine = 0.0f;
	UE_LOG(LogTemp,Warning, TEXT("StopPrimingThrow"))
}

/** Execute a throw if the throw is priming*/
void UPlayerPhysicsGrabComponent::PerformThrow()
{
	if(IsPrimingThrow)
	{
		
		FVector Target {FVector()};
		FVector TraceStart {Camera->GetComponentLocation()+RotatedHandOffset};
		FVector TraceEnd {Camera->GetForwardVector() * 1000 + TraceStart};FHitResult HitResult;
		FCollisionQueryParams CollisionParams;
		if  (this->GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, CollisionParams))
			{
				Target = HitResult.ImpactPoint;
			}
			else
			{
				Target = TraceEnd;
			}
		FVector ThrowDirection = (Target - TraceStart).GetSafeNormal();
		/** Calculate the throwing strenght using the timeline we updated in the tick.*/
		const float ThrowingStrength = FMath::Lerp(Configuration->MinThrowingStrength, Configuration->MaxThrowingStrength, FMath::Clamp((ThrowingTimeLine),0.0,1.0));
		
		GrabbedComponent->SetPhysicsLinearVelocity(ThrowDirection * ThrowingStrength);
		GrabbedComponent->WakeRigidBody();
		FString LogMessage = FString::Printf(TEXT("PerformThrow: %f"), ThrowingTimeLine);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *LogMessage);
		/** Release the grabbed component after the throw */
		ReleaseObject();
	}
}


void UPlayerPhysicsGrabComponent::UpdateRotatedHandOffset(FRotator& Rotation, FVector& HandOffset)
{
	/** Get the camera's world rotation. */
	CameraRotation = Camera->GetComponentRotation();

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
		TargetLocation = RotatedHandOffset * WillThrowOnReleaseMultiplier + (CurrentZoomLevel * Camera->GetForwardVector());

		FQuat CameraQuat = FQuat::MakeFromRotator(CameraRotation);
		FQuat MouseInputRotationQuat = FQuat::MakeFromRotator(FRotator(MouseInputRotation.Roll, MouseInputRotation.Pitch, MouseInputRotation.Yaw));
		//FQuat OriginalRotationQuat = FQuat::MakeFromRotator(FRotator(OriginalRotation.Roll, OriginalRotation.Pitch, OriginalRotation.Yaw));

		FVector CameraUp = Camera->GetUpVector();
		FVector CameraForward = Camera->GetForwardVector();
		FVector CameraRight = Camera->GetRightVector();


		
		FMatrix RotationMatrix(CameraRight, CameraForward, CameraUp, FVector::ZeroVector);
		FQuat ObjectRotation(RotationMatrix);
		/** Calculate the difference between the camera rotation and the original rotation */
		//RotationDifference = FRotator(0.0, CameraRotation.Yaw, 0.0);
		//TODO; HELP AAAAAAAAAAAAAAAA
		/** Update the rotation of the grabbed component based on the camera rotation */
		FRotator TargetRotation {};
		
		SetTargetLocationAndRotation(TargetLocation,TargetRotation);
	}
}

/** Updates on tick when you are manually rotating the object.*/
void UPlayerPhysicsGrabComponent::UpdateMouseImputRotation(FVector2d MouseInputDelta)
{
	MouseInputRotation += FRotator(MouseInputDelta.X,MouseInputDelta.Y, CurrentZoomAxisValue);
}
/** The update loop that scales the zoomaxis value from the mouse input */
void UPlayerPhysicsGrabComponent::UpdateZoomAxisValue(float ZoomAxis)
{
	if(!RotateObjectMode)
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

