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
				OnGrabbedObjectReleased.Broadcast(GrabbedComponent->GetOwner());
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
void UPlayerGrabComponent::GrabActor(AActor* ActorToGrab)
{
	/** check if there's a reference and cast to static mesh component to get a ref to the first static mesh. */
	if (!ActorToGrab || GrabbedComponent) { return; }
	if (UStaticMeshComponent* StaticMeshComponent {Cast<UStaticMeshComponent>(ActorToGrab->GetComponentByClass(UStaticMeshComponent::StaticClass()))})
	{
		CurrentZoomLevel = FVector::Distance(Camera->GetComponentLocation(), StaticMeshComponent->GetCenterOfMass());
		GrabComponentAtLocationWithRotation(StaticMeshComponent, NAME_None,StaticMeshComponent->GetCenterOfMass(),StaticMeshComponent->GetComponentRotation());

		/** Get the original rotation of the grabbed . */
		 OriginalRotation = StaticMeshComponent->GetRelativeRotation();
	
		/** Start the tick function so that the update for the target location can start updating. */
		SetComponentTickEnabled(true);

		/** Disable the colission with the player. */
		StaticMeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	}
	/** Reset the mouse rotation when you grab a new object*/
	MouseInputRotation = FRotator{0.0,0.0,0.0};

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
		if(WillThrowOnRelease)
		{
			GrabbedComponent->SetWorldLocation(Camera->GetComponentLocation() - 10 * Camera->GetForwardVector() + 10 * Camera->GetUpVector());
		}
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
		
		UE_LOG(LogGrabComponent, VeryVerbose, TEXT("%s"), *FString::Printf(TEXT("PerformThrow: %f"), ThrowingTimeLine));
		/** Release the grabbed component after the throw */
		ReleaseObject();
	}
}


void UPlayerGrabComponent::UpdateRotatedHandOffset(FRotator& Rotation, FVector& HandOffset)
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
void UPlayerGrabComponent::UpdateMouseImputRotation(FVector2d MouseInputDelta)
{
	MouseInputRotation += FRotator(MouseInputDelta.X,MouseInputDelta.Y, CurrentZoomAxisValue);
}
/** The update loop that scales the zoomaxis value from the mouse input */
void UPlayerGrabComponent::UpdateZoomAxisValue(float ZoomAxis)
{
	if(!IsRotationModeActive)
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

