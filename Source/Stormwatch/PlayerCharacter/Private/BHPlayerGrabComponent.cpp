// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHPlayerGrabComponent.h"
#include "KineticActorComponent.h"
#include "BHPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStaticsTypes.h"
#include "DrawDebugHelpers.h"
#include "EngineDefines.h"
#include "Components/PrimitiveComponent.h"
#include "PhysicsPublic.h"
#include "Physics/PhysicsInterfaceCore.h"
#include "Chaos/PBDJointConstraintTypes.h"
#include "Chaos/PBDJointConstraintData.h"
#include "ChaosCheck.h"

DEFINE_LOG_CATEGORY_CLASS(UBHPlayerGrabComponent, LogGrabComponent)

UBHPlayerGrabComponent::UBHPlayerGrabComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.TickGroup = TG_PrePhysics;
}

void UBHPlayerGrabComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBHPlayerGrabComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GrabbedComponent)
	{
		UpdateTargetLocationWithRotation(DeltaTime);

		if (CurrentZoomLevel != PreviousZoomLevel)
		{
			UpdatePhysicsHandle();
		}
		PreviousZoomLevel = CurrentZoomLevel;

		if (!bPrimingThrow)
		{
			/** Check if the distance between the location and target location is too big, let the object go. */
			if (LetGoDistance <= FVector::Distance(GrabbedComponent->GetComponentLocation(), TargetLocation))
			{
				ReleaseActor();
			}
		}
		else
		{
			UpdateThrowTimer(DeltaTime);
		}
	}
}

void UBHPlayerGrabComponent::UpdatePhysicsHandle()
{
	const float Alpha {static_cast<float>(FMath::GetMappedRangeValueClamped
	(FVector2D(MinZoomLevel, MaxZoomLevel), FVector2D(0,1), CurrentZoomLevel))};

	LinearDamping = FMath::Lerp(MinZoomLinearDamping, MaxZoomLinearDamping, Alpha);
	LinearStiffness = FMath::Lerp(MinZoomLinearStiffness, MaxZoomLinearStiffness, Alpha);
	AngularDamping = FMath::Lerp(MinZoomAngularDamping, MaxZoomAngularDamping, Alpha);
	AngularStiffness = FMath::Lerp(MinZoomAngularStiffness, MaxZoomAngularStiffness, Alpha);
	InterpolationSpeed = FMath::Lerp(MinZoomInterpolationSpeed, MaxZoomInterpolationSpeed, Alpha);

	if (!ConstraintHandle.IsValid() || !ConstraintHandle.Constraint->IsType(Chaos::EConstraintType::JointConstraintType)) { return; }
	
	FPhysicsCommand::ExecuteWrite(ConstraintHandle, [&](const FPhysicsConstraintHandle& InConstraintHandle)
	{
		if (Chaos::FJointConstraint * Constraint = static_cast<Chaos::FJointConstraint*>(ConstraintHandle.Constraint))
		{
			Constraint->SetLinearDriveStiffness(Chaos::FVec3(LinearStiffness));
			Constraint->SetLinearDriveDamping(Chaos::FVec3(LinearDamping));

			Constraint->SetAngularDriveStiffness(Chaos::FVec3(AngularStiffness));
			Constraint->SetAngularDriveDamping(Chaos::FVec3(AngularDamping));
		}
	});
}

void UBHPlayerGrabComponent::UpdateRotatedHandOffset(FRotator& Rotation, FVector& HandOffset)
{
	UCameraComponent* Camera = GetPlayerCharacter()->GetCamera();

	CameraRotation = Camera->GetComponentRotation();
	CameraQuat = Camera->GetComponentQuat();

	const FVector MinValues = FVector(-ThrowingShakeSize,-ThrowingShakeSize,-ThrowingShakeSize);
	const FVector MaxValues = FVector(ThrowingShakeSize,ThrowingShakeSize,ThrowingShakeSize);
	const FVector ThrowingShake = FMath::RandPointInBox(FBox(MinValues, MaxValues));
	const FVector ThrowingVector = (ThrowingShake + ThrowingBackupVector) * FMath::Clamp((ThrowingTimeLine),0.0,1.0);
	
	RotatedHandOffset =CameraRotation.RotateVector(RelativeHoldingHandLocation + ThrowingVector);

	const float HandOffsetScalar {static_cast<float>(FMath::Clamp((((BeginHandOffsetDistance)
		- CurrentZoomLevel) / (BeginHandOffsetDistance + BeginHandOffsetDistance * GrabbedComponentSize)), 0.0, 1000.0))};

	FVector NormalizedRotatedHandOffset = RotatedHandOffset.GetSafeNormal();

	float OffsetScalar = HandOffsetScalar * GrabbedComponentSize;

	FVector RotatedScaledHandOffset = OffsetScalar * (RotatedHandOffset + NormalizedRotatedHandOffset * GrabbedComponentSize);

	RotatedHandOffset = Camera->GetComponentLocation() + RotatedScaledHandOffset;
}

/** The looping function that updates the target location and rotation of the currently grabbed object*/
void UBHPlayerGrabComponent::UpdateTargetLocationWithRotation(float DeltaTime)
{
	if (!GrabbedComponent) { return; }

	UCameraComponent* Camera = GetPlayerCharacter()->GetCamera();
	UBHPlayerMovementComponent* MovementComponent = GetPlayerCharacter()->GetPlayerMovement();
	
	if (MovementComponent && MovementComponent->GetIsSprinting())
	{
		CurrentZoomLevel = CurrentZoomLevel - ReturnZoomSpeed * DeltaTime;
	}
	else
	{
		CurrentZoomLevel = CurrentZoomLevel + CurrentZoomAxisValue * ZoomSpeed * DeltaTime;
	}
		
	CurrentZoomLevel = FMath::Clamp(CurrentZoomLevel, MinZoomLevel, MaxZoomLevel);
	
	UpdateRotatedHandOffset(CameraRotation, RotatedHandOffset);
	TargetLocation = RotatedHandOffset + (CurrentZoomLevel * Camera->GetForwardVector());

	FRotator TargetRotation = FRotator(CameraQuat * CameraRelativeRotation);
	if (CurrentZoomLevel > BeginHandOffsetDistance)
	{
		//TODO; Slerp the rotation to a surface.
	}
	SetTargetLocationAndRotation(TargetLocation, TargetRotation);
}

void UBHPlayerGrabComponent::UpdateMouseInputRotation(FVector2d MouseInputDelta)
{
	if(bRotatingActor)
	{
		FQuat MouseInputQuatX = FQuat(1,0,0,CurrentRotationZoomAxisValue*0.07);
		FQuat MouseInPutQuatY = FQuat(0,1,0,-MouseInputDelta.Y*0.01);
		FQuat MouseInPutQuatZ = FQuat(0,0,1,MouseInputDelta.X*0.01);
		FQuat DeltaRotation = MouseInputQuatX * MouseInPutQuatY * MouseInPutQuatZ;
		
		/**Normalize this rotation and apply it to the relative rotation of the object.*/
		CameraRelativeRotation = DeltaRotation.GetNormalized() * CameraRelativeRotation;
	}
}

void UBHPlayerGrabComponent::UpdateThrowTimer(float DeltaTime)
{
	PerformThrow(1);

	if (PrePrimingThrowTimer <= PrimeDelay)
	{
		PrePrimingThrowTimer += DeltaTime;
	}
	else
	{
		bThrowOnRelease = true;
			
		if (CurrentZoomLevel != ThrowingZoomLevel)
		{
			CurrentZoomLevel += ToThrowingZoomSpeed * (ThrowingZoomLevel - CurrentZoomLevel) *0.001;
		}
			
		if (PrePrimingThrowTimer <= 1.0)
		{
			ThrowingTimeLine += FMath::Clamp(DeltaTime/ThrowChargeTime, 0.0,1.0);
		}
	}
}

/** The update loop that scales the zoomaxis value from the mouse input */
void UBHPlayerGrabComponent::UpdateZoomAxisValue(float ZoomAxis)
{
	if(!bRotatingActor)
	{
		CurrentZoomAxisValue = FMath::Clamp(((CurrentZoomAxisValue + 0.1 * ZoomAxis) * 0.9),-2.0,2.0);
	}
	else
	{
		CurrentRotationZoomAxisValue = ZoomAxis;
	}
	
}

void UBHPlayerGrabComponent::GrabActor(AActor* ActorToGrab)
{
	if (!ActorToGrab || GrabbedComponent) { return; }

	UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(ActorToGrab->GetComponentByClass(UStaticMeshComponent::StaticClass()));
	if (!StaticMeshComponent)
	{
		UE_LOG(LogGrabComponent, Warning, TEXT("Attempted to grab an Actor with no valid StaticMeshComponent."));
		return;
	}

	UCameraComponent* Camera = GetPlayerCharacter()->GetCamera();

	CurrentZoomLevel = FVector::Distance(Camera->GetComponentLocation(), StaticMeshComponent->GetCenterOfMass());
	GrabComponentAtLocationWithRotation(StaticMeshComponent, NAME_None,StaticMeshComponent->GetCenterOfMass(),StaticMeshComponent->GetComponentRotation());

		
	CameraRelativeRotation = Camera->GetComponentQuat().Inverse() * GrabbedComponent->GetComponentQuat();
	
	/** Check if the actor already has a kinetic component. If this is the case, call HandleOnOwnerGrabbed on the component.
	 *	If not, add the component to the grabbed actor. */
	if (UKineticActorComponent* KineticComponent = Cast<UKineticActorComponent>(ActorToGrab->GetComponentByClass(UKineticActorComponent::StaticClass()));)
	{
		KineticComponent->HandleOnOwnerGrabbed();
	}
	else
	{
		ActorToGrab->AddComponentByClass(UKineticActorComponent::StaticClass(), false, FTransform(), false);
	}
	
	FBox BoundingBox = GrabbedComponent->Bounds.GetBox();
	GrabbedComponentSize = FVector::Distance(BoundingBox.Min, BoundingBox.Max)/2;

	/** Start the enable tick so that the update for the target location can start updating. */
	SetComponentTickEnabled(true);
}

void UBHPlayerGrabComponent::ReleaseActor()
{
	if (!GrabbedComponent) { return; }

	SetComponentTickEnabled(false);

	if (UKineticActorComponent* KineticComponent = Cast<UKineticActorComponent>(GrabbedComponent->GetOwner()->GetComponentByClass(UKineticActorComponent::StaticClass()));)
	{
		KineticComponent->HandleOnOwnerReleased();
	}
		
	if(!bThrowOnRelease)
	{
		/** If the object is not thrown, clamp the velocity.*/
		FVector Velocity = GrabbedComponent->GetComponentVelocity();
		GrabbedComponent->SetPhysicsLinearVelocity(Velocity.GetClampedToSize(0.0, 500.0));
	}

	bThrowOnRelease = false;
	ReleaseComponent();
	StopPrimingThrow();
}


void UBHPlayerGrabComponent::PrimeThrow()
{
	bPrimingThrow = true;
	PrePrimingThrowTimer = 0.0;
	ThrowingTimeLine = 0.0f;
	UE_LOG(LogGrabComponent, VeryVerbose, TEXT("Started Priming Throw."))
}

void UBHPlayerGrabComponent::StopPrimingThrow()
{
	bPrimingThrow = false;
	bThrowOnRelease = false;
	PrePrimingThrowTimer = 0.0;
	ThrowingTimeLine = 0.0f;
	UE_LOG(LogGrabComponent, VeryVerbose, TEXT("Stopped Priming Throw."))
}

/** Execute a throw if the throw is priming*/
void UBHPlayerGrabComponent::PerformThrow(bool bOnlyPreviewTrajectory)
{
	if (!bThrowOnRelease) { return; }
	
	const UCameraComponent* Camera = GetPlayerCharacter()->GetCamera();
		
	bool ThrowOverHands = false;

	/** Calculate the throwing strenght using the timeline we updated in the tick.*/
	const float ThrowingStrength = ThrowStrengthCurve->GetFloatValue(ThrowingTimeLine);

	FVector Target = FVector();
	const FVector TraceStart = Camera->GetComponentLocation();
	const FVector TraceEnd = Camera->GetForwardVector() * 10000000 + TraceStart;FHitResult HitResult;

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredComponent(GrabbedComponent);
	if  (this->GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, CollisionParams))
	{
		Target = HitResult.ImpactPoint;
	}
	else
	{
		Target = TraceEnd;
	}
		
	/** Calculate the direction from the player to the target */
	FVector Direction = Target - GrabbedComponent->GetComponentLocation();
	Direction.Normalize();
		
	FVector FinalDirection = 0,0,0;
	if(Target != TraceEnd)
	{
		/** Calculate the angle to throw the object using a ballistic trajectory.*/
		//float ThrowAngle = CalculateThrowAngle(ReleaseLocation,Target,ThrowingStrength,ThrowOverHands);
			
		/**Rotate the Direction vector around the cross product of the Direction vector and the world up vector by the angle Theta. */
		FVector RotationAxis = Direction.CrossProduct(Direction,FVector::UpVector);
		RotationAxis.Normalize();
		//FinalDirection = Direction.RotateAngleAxis(ThrowAngle, RotationAxis);
		FinalDirection.Normalize();

		float VectorLength = RotationAxis.Size();
	}
	else
	{
		FinalDirection = Direction;
	}

	/** for now, we won't use the manual calculation for throwing angle if there isn't any solution*/
	FinalDirection = Direction;
		
	ThrowVelocity = FinalDirection * ThrowingStrength;

	FVector TossVelocity;
	if(!UGameplayStatics::SuggestProjectileVelocity(
			this,
			TossVelocity,
			GrabbedComponent->GetComponentLocation(),
			Target,
			ThrowingStrength,
			false,
			0,
			GetWorld()->GetGravityZ(),
			ESuggestProjVelocityTraceOption::DoNotTrace,
			FCollisionResponseParams::DefaultResponseParam,
			TArray<AActor*>= GetOwner(),GetGrabbedActor(),
			false))
	{
		TossVelocity = ThrowVelocity;
	}
	VisualizeProjectilePath(GrabbedComponent->GetOwner(),GrabbedComponent->GetComponentLocation(),TossVelocity);

	if(!bOnlyPreviewTrajectory)
	{
		GrabbedComponent->WakeRigidBody();
		ReleaseActor();
		GrabbedComponent->SetPhysicsLinearVelocity(TossVelocity);
	}
}

void UBHPlayerGrabComponent::VisualizeProjectilePath(AActor* ProjectileActor, FVector StartLocation, FVector LaunchVelocity)
{
	/** Define the parameters for the prediction */
	TArray<FVector> OutPathPositions;
	FPredictProjectilePathResult OutPath;
	FPredictProjectilePathParams PredictParams{};
	PredictParams.StartLocation = StartLocation;
	PredictParams.LaunchVelocity = LaunchVelocity;
	PredictParams.bTraceComplex = false;
	PredictParams.bTraceWithCollision = true;
	PredictParams.DrawDebugType = EDrawDebugTrace::None;
	PredictParams.DrawDebugTime = 2.0f;
	PredictParams.SimFrequency = 30.0f;
	PredictParams.MaxSimTime = 2.0f;
	PredictParams.ActorsToIgnore = TArray<AActor*> = GetOwner(),GrabbedComponent->GetOwner();

	/** Call the prediction function */
	UGameplayStatics::PredictProjectilePath(ProjectileActor,PredictParams,OutPath);

	if (OutPath.HitResult.bBlockingHit)
	{
		FVector HitLocation = OutPath.HitResult.Location;
		float SphereRadius = 10.0f;
		FColor SphereColor = FColor::Red;
		float SphereLifeTime = 0.0f;
		DrawDebugSphere(GetWorld(), HitLocation, SphereRadius, 32, SphereColor, false, SphereLifeTime);
	}

	
	/** for (int32 i = 0; i < OutPathPositions.Num(); ++i)
	{
		const FVector& PathPosition = OutPathPositions[i];
		const FPredictProjectilePathPointData& PathPointData = OutPath.PathData[i];
	} */
}

void UBHPlayerGrabComponent::BeginTetriaryInteraction()
{
	bRotatingActor = true;
}

void UBHPlayerGrabComponent::EndTetriaryInteraction()
{
	bRotatingActor = false;
}






