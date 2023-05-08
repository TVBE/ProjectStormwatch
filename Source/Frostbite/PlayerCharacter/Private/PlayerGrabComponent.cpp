// Copyright (c) 2022-present Barrelhouse
// Written by Nino Saglia
// This source code is part of the project Frostbite

#include "PlayerGrabComponent.h"
#include "KineticActorComponent.h"
#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStaticsTypes.h"

#include "DrawDebugHelpers.h"


DEFINE_LOG_CATEGORY_CLASS(UPlayerGrabComponent, LogGrabComponent)

void UPlayerGrabComponent::OnRegister()
{
	Super::OnRegister();

	if (APlayerCharacter* PlayerCharacter {Cast<APlayerCharacter>(GetOwner())})
	{
		Camera = PlayerCharacter->GetCamera();
		Movement = PlayerCharacter->GetPlayerCharacterMovement();
		Player = PlayerCharacter;
		
	}

	UWorld* World = GetWorld();
	if (World != nullptr)
	{
		Gravity = World->GetGravityZ();
	}
}

void UPlayerGrabComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GrabbedComponent)
	{
		UpdateTargetLocationWithRotation(DeltaTime);

		// UpdateCameraRotationSpeed(DeltaTime);

		UpdateThrowTimer(DeltaTime);
		
	}
}

void UPlayerGrabComponent::BeginPlay()
{
	Super::BeginPlay();
	SetComponentTickEnabled(false);
}

void UPlayerGrabComponent::UpdateCameraRotationSpeed(float DeltaTime)
{
	/** The camera should move slower when the player is holding an object away from himself.*/
	if(GrabbedComponent)
	{
		CameraRotationMultiplier =
			1 /(((CurrentZoomLevel-Configuration->MinZoomLevel)
					/Configuration->MaxZoomLevel)
					* FMath::Clamp(GrabbedComponent->GetMass(), 10.0, 200.0) //Claming the mass so that there's always some noticable difference in speed and never feels too slow.
					* Configuration->CameraRotationDecreasingStrength);
	}
	else
	{
		CameraRotationMultiplier = 1.0;
	}
}

void UPlayerGrabComponent::UpdateThrowTimer(float DeltaTime)
{

	if (Configuration)
	{
		if (!IsPrimingThrow)
		{
			/** Check if the distance between the location and target location is too big, let the object go. */
			if (Configuration->LetGoDistance <= FVector::Distance(GrabbedComponent->GetComponentLocation(), TargetLocation))
			{
				ReleaseObject();
			}
		}
		if (IsPrimingThrow)
		{

			PerformThrow(true);
			if (PrePrimingThrowTimer <= Configuration->PrePrimingThrowDelayTime)
			{
				PrePrimingThrowTimer += DeltaTime;
			}
			else
			{
				WillThrowOnRelease = true;
					
				if (CurrentZoomLevel != Configuration->ThrowingZoomLevel)
				{
					CurrentZoomLevel += Configuration->ToThrowingZoomSpeed * (Configuration->ThrowingZoomLevel - CurrentZoomLevel);
				}
					
				if (PrePrimingThrowTimer <= 1.0)
				{
					/**The timeline to be used by various on tick functions that update location.
					*We normalize the value here since it otherwise would be need to be normalised multiple times later.
					*/
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
	if (!ActorToGrab || GrabbedComponent)
		{
			UE_LOG(LogGrabComponent, Warning, TEXT("No actor references"));
			return;
		}
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
	else
	{
		UE_LOG(LogGrabComponent,Warning,TEXT("NoStaticmeshFound"))
	}


	/** Check if the actor already has a kinetic component. If this is the case, call HandleOnOwnerGrabbed on the component.
	 *	If not, add the component to the grabbed actor. */
	if (UKineticActorComponent* KineticComponent {Cast<UKineticActorComponent>(ActorToGrab->GetComponentByClass(UKineticActorComponent::StaticClass()))})
	{
		KineticComponent->HandleOnOwnerGrabbed();
	}
	else
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

		if (UKineticActorComponent* KineticComponent {Cast<UKineticActorComponent>(GrabbedComponent->GetOwner()->GetComponentByClass(UKineticActorComponent::StaticClass()))})
		{
			KineticComponent->HandleOnOwnerReleased();
		}
		
		if(WillThrowOnRelease)
		{
			GrabbedComponent->SetWorldLocation(ReleaseLocation);
		}
		else
		{
			/** If the object is not thrown, clamp the velocity.*/
			FVector Velocity = GrabbedComponent->GetComponentVelocity();
			GrabbedComponent->SetPhysicsLinearVelocity(Velocity.GetClampedToSize(0.0, 500.0));
		}
		WillThrowOnRelease = false;
		ReleaseComponent();
		UE_LOG(LogGrabComponent, VeryVerbose, TEXT("Released Object."))
		StopPrimingThrow();
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
void UPlayerGrabComponent::PerformThrow(bool OnlyPreviewTrajectory)
{
	if(WillThrowOnRelease)
	{
		
		bool ThrowOverHands{false};
		/** Calculate the throwing strenght using the timeline we updated in the tick.*/
		const float ThrowingStrength{Configuration->ThrowingStrengthCure->GetFloatValue(ThrowingTimeLine)};
		if(ThrowingTimeLine >0.35)
		{
			ReleaseLocation = Camera->GetComponentLocation() + 10 * Camera->GetForwardVector() + 10 * Camera->GetUpVector();
			ThrowOverHands = true;
		}
		else
		{
			ReleaseLocation = RotatedHandOffset;
		}
		
		FVector Target {FVector()};
		FVector TraceStart {Camera->GetComponentLocation()};
		FVector TraceEnd {Camera->GetForwardVector() * 10000000 + TraceStart};FHitResult HitResult;
		FCollisionQueryParams CollisionParams;
		if  (this->GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, CollisionParams))
		{
			Target = HitResult.ImpactPoint;
		}
		else
		{
			Target = TraceEnd;
		}
		
		
		/** Calculate the direction from the player to the target */
		FVector Direction = Target - ReleaseLocation;
		Direction.Normalize();
		

		FVector FinalDirection{0,0,0};
		if(Target != TraceEnd)
		{
			/** Calculate the angle to throw the object using a ballistic trajectory.*/
			float ThrowAngle = CalculateThrowAngle(ReleaseLocation,Target,ThrowingStrength,ThrowOverHands);
			
			/**Rotate the Direction vector around the cross product of the Direction vector and the world up vector by the angle Theta. */
			FVector RotationAxis = Direction.CrossProduct(Direction,FVector::UpVector);
			RotationAxis.Normalize();
			FinalDirection = Direction.RotateAngleAxis(ThrowAngle, RotationAxis);
			FinalDirection.Normalize();

			float VectorLength = RotationAxis.Size();
		}
		else
		{
			FinalDirection = Direction;
		}

		
		
		

		
		ThrowVelocity = FinalDirection * ThrowingStrength;

		FVector TossVelocity;
		if(!UGameplayStatics::SuggestProjectileVelocity(
				this,
				TossVelocity,
				ReleaseLocation,
				Target,
				ThrowingStrength,
				false,
				0,
				Gravity,
				ESuggestProjVelocityTraceOption::DoNotTrace,
				FCollisionResponseParams::DefaultResponseParam,
				TArray<AActor*>{Player},
				true))
		{
			TossVelocity = ThrowVelocity;
		}
		
		if(!OnlyPreviewTrajectory)
		{
			/** Set the physics velocity of the grabbed component to the calculated throwing direction */
			
			GrabbedComponent->SetPhysicsLinearVelocity(TossVelocity);
			/** Release the grabbed component after the throw */
			GrabbedComponent->SetWorldLocation(ReleaseLocation);
			GrabbedComponent->WakeRigidBody();
			ReleaseObject();
			
		}
	}
}

/** Calculate the angle to throw the object using a ballistic trajectory.*/
float UPlayerGrabComponent::CalculateThrowAngle(FVector StartLocation, FVector Target, float Velocity, bool overhands)
{
	/** Calculate displacement vector D */
	FVector D{Target - StartLocation};
	D = D*1.0; //Some scaling to aproximately compensate for air resistance.
	if(!overhands)
	{
		D = D*1.0;// when throwing underhands the item will generally experience more air resistance.
	}

	/** Calculate horizontal distance HD to target location */
	float HD{float (FVector::DistXY(StartLocation, Target))};
	
	/** Calculate initial velocity as I required to hit the target */
	const float G{abs(Gravity)}; // acceleration due to gravity as G.
	float Theta{atan((G * HD) / (Velocity * Velocity))};
	float I{Velocity / cos(Theta)};

	/** Solve for the angle theta for a balistic trajectory hitting target location with the given velocity.
	/** the calculation of the low trajectory that solves the requirements.*/
	Theta = atan((I * I - sqrt(I * I * I * I - G * (G * HD * HD + 0.0*D.Z * I * I))) / (G * HD));

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
	if (!GrabbedComponent || !Configuration) { return; }
	AActor* CompOwner = this->GetOwner();
	
	if (CompOwner)
	{
		if (Movement && Movement->GetIsSprinting())
		{
			CurrentZoomLevel = CurrentZoomLevel - Configuration->WalkingRetunZoomSpeed * DeltaTime;
		}
		else
		{
			CurrentZoomLevel = CurrentZoomLevel + CurrentZoomAxisValue * Configuration->ZoomSpeed * DeltaTime;
		}
		
		CurrentZoomLevel = FMath::Clamp(CurrentZoomLevel, Configuration->MinZoomLevel, Configuration->MaxZoomLevel);
	}
	
	if (Camera)
	{
		UpdateRotatedHandOffset(CameraRotation, RotatedHandOffset);
		TargetLocation = RotatedHandOffset + (CurrentZoomLevel * Camera->GetForwardVector());

		FRotator TargetRotation{FRotator(CameraQuat * CameraRelativeRotation)};
		if (CurrentZoomLevel > Configuration->BeginHandOffsetDistance)
		{
			//TODO; Slerp the rotation to a surface.
		}
		SetTargetLocationAndRotation(TargetLocation, TargetRotation);
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

void UPlayerGrabComponent::ApplyToPhysicsHandle()
{
	// Set the member variables of this PhysicsHandleComponent to the values in this data asset.

	SetLinearDamping(Configuration->LinearDamping);
	SetLinearStiffness(Configuration->LinearStiffness);
	SetAngularDamping(Configuration->AngularDamping);
	SetAngularStiffness(Configuration->AngularStiffness);
	SetInterpolationSpeed(Configuration->InterpolationSpeed);
}

