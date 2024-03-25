// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHPlayerGrabComponent.h"

#include "BHGrabbableObjectInterface.h"
#include "BHPhysicsInteractableComponent.h"
#include "BHPlayerCameraComponent.h"
#include "BHPlayerCharacter.h"
#include "BHCharacterMovementComponent.h"
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

DEFINE_LOG_CATEGORY_STATIC(LogBHPlayerGrabComponent, Log, All)

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
	if (!IsHoldingObject())
	{
		SetComponentTickEnabled(false);
		UE_LOG(LogBHPlayerGrabComponent, Warning, TEXT("Tick: No object is currently being held. Tick is disabled."))
		return;
	}
	
	UpdateHeldObjectTargetTransform(DeltaTime);
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	// START DEPRECATED
	// if (GrabbedComponent)
	// {
	// 	UpdateHeldObjectTargetTransform(DeltaTime);
	//
	// 	if (TargetDistanceFromView != ObjectLastDistanceFromView)
	// 	{
	// 		UpdatePhysicsHandle();
	// 	}
	// 	ObjectLastDistanceFromView = TargetDistanceFromView;
	//
	// 	if (!bPrimingThrow)
	// 	{
	// 		/** Check if the distance between the location and target location is too big, let the object go. */
	// 		if (LetGoDistance <= FVector::Distance(GrabbedComponent->GetComponentLocation(), TargetLocation))
	// 		{
	// 			ReleaseActor();
	// 		}
	// 	}
	// 	else
	// 	{
	// 		UpdateThrowTimer(DeltaTime);
	// 	}
	// }
}

void UBHPlayerGrabComponent::Grab(UObject* Object)
{
	if(!ensureAlwaysMsgf(!IsHoldingObject(), TEXT("Grab: An object is already being grabbed.")))
	{
		return;
	}
	if (!IsValid(Object))
	{
		UE_LOG(LogBHPlayerGrabComponent, Warning, TEXT("Grab: No valid object provided."))
		return;
	}
	if (!Object->Implements<UBHGrabbableObject>())
	{
		UE_LOG(LogBHPlayerGrabComponent, Warning, TEXT("Grab: Object [%s] does not implement IBHGrabbableObject."),
		*Object->GetName());
		return;
	}
	
	UPrimitiveComponent* GrabbableComponent = nullptr;
	if (UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(Object);
		PrimitiveComponent->IsSimulatingPhysics())
	{
		GrabbableComponent = PrimitiveComponent;
	}

	// If we lack a grabbable component at this point, due to the object possibly being an Actor or Actor Component,
	// we'll seek the first grabbable Primitive Component within the Actor.
	if (!GrabbableComponent)
	{
		const AActor* Actor = Cast<AActor>(Object);
		if (!Actor)
		{
			if (const UActorComponent* Component = Cast<UActorComponent>(Object))
			{
				Actor = Component->GetOwner();
			}
		}
		if (Actor)
		{
			if (bAllowRootOnly)
			{
				if (UPrimitiveComponent* Root = Cast<UPrimitiveComponent>(Actor->GetRootComponent());
					Root->IsSimulatingPhysics())
				{
					GrabbableComponent = Root;
				}
			}
			else
			{
				TArray<UPrimitiveComponent*> Components;
				Actor->GetComponents<UPrimitiveComponent>(Components);
				GrabbableComponent = *Algo::FindByPredicate(Components,
					[](const UPrimitiveComponent* Component)
					{
						return IsValid(Component) && Component->IsSimulatingPhysics();
					}
				);
			}
		}
	}

	if (!GrabbableComponent)
	{
		UE_LOG(LogBHPlayerGrabComponent, Warning, TEXT("Grab: Object [%s] is not a grabbable."),
		*Object->GetName());
		return;
	}

	// Check if grabbable component is within reach and in front of the player to avoid snapping issues.
	// We don't expect large deviations here, so we exit if this is the case.
	if (!IsWithinGrabbableRange(GrabbableComponent))
	{
		UE_LOG(LogBHPlayerGrabComponent, Verbose, TEXT("Grab: Component [%s] is not within grabbable reach."),
		*Object->GetName());
		return;
	}

	// Grab the object if the object lets us.
	// Calling this commits us; we can't abort after due to unknown object behavior.
	const bool bGrabbed = IBHGrabbableObject::Execute_Grab(Object, GetOwner());
	if (!bGrabbed)
	{
		UE_LOG(LogBHPlayerGrabComponent, Verbose, TEXT("Grab: Object [%s] returned false for IBHGrabbableObject::Grab."),
			*Object->GetName());
		return;
	}
	
	const UBHPlayerCameraComponent* Camera = GetPlayerCharacter()->GetCamera();
	ObjectRelativeRotation = Camera->GetComponentQuat().Inverse() * GrabbedComponent->GetComponentQuat();
	
	FBox BoundingBox = GrabbedComponent->Bounds.GetBox();
	GrabbedComponentSize = FVector::Distance(BoundingBox.Min, BoundingBox.Max)/2;
	
	GrabComponentAtLocationWithRotation(GrabbableComponent, NAME_None,GrabbableComponent->GetCenterOfMass(),
		GrabbableComponent->GetComponentRotation());
	OnObjectGrabbed.Broadcast(Object, GrabbableComponent);
	SetComponentTickEnabled(true);

	UE_LOG(LogBHPlayerGrabComponent, Verbose, TEXT("Grab: Grabbed Object [%s], component [%s]."),
		*Object->GetName(), *GrabbableComponent->GetName());
}

void UBHPlayerGrabComponent::Release()
{
	if (!IsHoldingObject())
	{
		UE_LOG(LogBHPlayerGrabComponent, Verbose, TEXT("Release: No object is being held."));
		return;
	}
	
	IBHGrabbableObject::Execute_Release(GrabbedObject, GetOwner());
	
	const UPrimitiveComponent* ReleasedComponent = GetGrabbedComponent();
	const UObject* ReleasedObject = GrabbedObject;
	ReleaseComponent();
	ObjectRelativeRotation.Reset();
	
	OnObjectReleased.Broadcast(ReleasedObject, ReleasedComponent);
	SetComponentTickEnabled(false);

	UE_LOG(LogBHPlayerGrabComponent, Verbose, TEXT("Grab: Released Object [%s], component [%s]."),
	*ReleasedObject->GetName(), *ReleasedComponent->GetName());
}

void UBHPlayerGrabComponent::Equip(UObject* Object)
{
	if (!IsValid(Object))
	{
		UE_LOG(LogBHPlayerGrabComponent, Warning, TEXT("Equip: No valid object provided."));
		return;
	}
	if (IsHoldingObject() && GrabbedObject != Object)
	{
		UE_LOG(LogBHPlayerGrabComponent, Warning, TEXT("Equip: Object [%s] is not the object currently being held."),
			*Object->GetName());
		return;
	}
	// If we haven't grabbed the object, we will try to grab it now.
	if (!IsHoldingObject())
	{
		Grab(Object);
		if (!IsHoldingObject())
		{
			return;
		}
	}

	IBHGrabbableObject::Execute_Equip(Object, GetOwner());
	OnObjectEquipped.Broadcast(Object, GrabbedComponent);

	UE_LOG(LogBHPlayerGrabComponent, Verbose, TEXT("Equip: Equipped Object [%s], component [%s]."),
		*Object->GetName(), *GrabbedComponent->GetName());
}

void UBHPlayerGrabComponent::Unequip()
{
}

void UBHPlayerGrabComponent::UpdatePhysicsHandle()
{
	// const float Alpha {static_cast<float>(FMath::GetMappedRangeValueClamped
	// (FVector2D(MinZoomLevel, MaxZoomLevel), FVector2D(0,1), TargetDistanceFromView))};
	//
	// LinearDamping = FMath::Lerp(MinZoomLinearDamping, MaxZoomLinearDamping, Alpha);
	// LinearStiffness = FMath::Lerp(MinZoomLinearStiffness, MaxZoomLinearStiffness, Alpha);
	// AngularDamping = FMath::Lerp(MinZoomAngularDamping, MaxZoomAngularDamping, Alpha);
	// AngularStiffness = FMath::Lerp(MinZoomAngularStiffness, MaxZoomAngularStiffness, Alpha);
	// InterpolationSpeed = FMath::Lerp(MinZoomInterpolationSpeed, MaxZoomInterpolationSpeed, Alpha);
	//
	// if (!ConstraintHandle.IsValid() || !ConstraintHandle.Constraint->IsType(Chaos::EConstraintType::JointConstraintType)) { return; }
	//
	// FPhysicsCommand::ExecuteWrite(ConstraintHandle, [&](const FPhysicsConstraintHandle& InConstraintHandle)
	// {
	// 	if (Chaos::FJointConstraint * Constraint = static_cast<Chaos::FJointConstraint*>(ConstraintHandle.Constraint))
	// 	{
	// 		Constraint->SetLinearDriveStiffness(Chaos::FVec3(LinearStiffness));
	// 		Constraint->SetLinearDriveDamping(Chaos::FVec3(LinearDamping));
	//
	// 		Constraint->SetAngularDriveStiffness(Chaos::FVec3(AngularStiffness));
	// 		Constraint->SetAngularDriveDamping(Chaos::FVec3(AngularDamping));
	// 	}
	// });
}

void UBHPlayerGrabComponent::UpdateRotatedHandOffset(FRotator& Rotation, FVector& HandOffset)
{
	// UCameraComponent* Camera = GetPlayerCharacter()->GetCamera();
	//
	// CameraRotation = Camera->GetComponentRotation();
	// CameraQuat = Camera->GetComponentQuat();
	//
	// const FVector MinValues = FVector(-ThrowingShakeSize,-ThrowingShakeSize,-ThrowingShakeSize);
	// const FVector MaxValues = FVector(ThrowingShakeSize,ThrowingShakeSize,ThrowingShakeSize);
	// const FVector ThrowingShake = FMath::RandPointInBox(FBox(MinValues, MaxValues));
	// const FVector ThrowingVector = (ThrowingShake + ThrowingBackupVector) * FMath::Clamp((ThrowingTimeLine),0.0,1.0);
	//
	// RotatedHandOffset =CameraRotation.RotateVector(RelativeHoldingHandLocation + ThrowingVector);
	//
	// const float HandOffsetScalar {static_cast<float>(FMath::Clamp((((BeginHandOffsetDistance)
	// 	- TargetDistanceFromView) / (BeginHandOffsetDistance + BeginHandOffsetDistance * GrabbedComponentSize)), 0.0, 1000.0))};
	//
	// FVector NormalizedRotatedHandOffset = RotatedHandOffset.GetSafeNormal();
	//
	// float OffsetScalar = HandOffsetScalar * GrabbedComponentSize;
	//
	// FVector RotatedScaledHandOffset = OffsetScalar * (RotatedHandOffset + NormalizedRotatedHandOffset * GrabbedComponentSize);
	//
	// RotatedHandOffset = Camera->GetComponentLocation() + RotatedScaledHandOffset;
}

bool UBHPlayerGrabComponent::IsWithinGrabbableRange(const UPrimitiveComponent* Component) const
{
	return true;
}

/** The looping function that updates the target location and rotation of the currently grabbed object*/
void UBHPlayerGrabComponent::UpdateHeldObjectTargetTransform(float DeltaTime)
{
	// const ABHPlayerCharacter* Character = GetPlayerCharacter();
	// if (!IsValid(Character))
	// {
	// 	return;
	// }
	//
	// TargetDistanceFromView = 
	//
	// if (Character->GetPlayerMovementComponent()->IsSprinting())
	// {
	// 	TargetDistanceFromView = TargetDistanceFromView - ReturnZoomSpeed * DeltaTime;
	// }
	// else
	// {
	// 	TargetDistanceFromView = TargetDistanceFromView + ControlZoomValue * ZoomSpeed * DeltaTime;
	// }
	// 	
	// TargetDistanceFromView = FMath::Clamp(TargetDistanceFromView, MinZoomLevel, MaxZoomLevel);
	//
	// UpdateRotatedHandOffset(CameraRotation, RotatedHandOffset);
	// TargetLocation = RotatedHandOffset + (TargetDistanceFromView * Camera->GetForwardVector());
	//
	// FRotator TargetRotation = FRotator(CameraQuat * ObjectRelativeRotation);
	// if (TargetDistanceFromView > BeginHandOffsetDistance)
	// {
	// 	//TODO; Slerp the rotation to a surface.
	// }
	// SetTargetLocationAndRotation(TargetLocation, TargetRotation);
}

void UBHPlayerGrabComponent::UpdateMouseInputRotation(FVector2d MouseInputDelta)
{
	// if(bRotatingActor)
	// {
	// 	FQuat MouseInputQuatX = FQuat(1,0,0,CurrentRotationZoomAxisValue*0.07);
	// 	FQuat MouseInPutQuatY = FQuat(0,1,0,-MouseInputDelta.Y*0.01);
	// 	FQuat MouseInPutQuatZ = FQuat(0,0,1,MouseInputDelta.X*0.01);
	// 	FQuat DeltaRotation = MouseInputQuatX * MouseInPutQuatY * MouseInPutQuatZ;
	// 	
	// 	/**Normalize this rotation and apply it to the relative rotation of the object.*/
	// 	ObjectRelativeRotation = DeltaRotation.GetNormalized() * ObjectRelativeRotation;
	// }
}

bool UBHPlayerGrabComponent::IsHoldingObject() const
{
	return IsValid(GrabbedComponent);
}

AActor* UBHPlayerGrabComponent::GetGrabbedActor() const
{
	const UPrimitiveComponent* PrimitiveComponent = GetGrabbedComponent();
	return PrimitiveComponent ? PrimitiveComponent->GetOwner() : nullptr;
}

UObject* UBHPlayerGrabComponent::GetGrabbedObject() const
{
	return GrabbedObject;
}

bool UBHPlayerGrabComponent::IsPrimingThrow() const
{
	return bPrimingThrow;
}

bool UBHPlayerGrabComponent::WillThrowOnRelease() const
{
	return bThrowOnRelease; 
}

void UBHPlayerGrabComponent::UpdateThrowTimer(float DeltaTime)
{
	// Throw(1);
	//
	// if (PrePrimingThrowTimer <= PrimeDelay)
	// {
	// 	PrePrimingThrowTimer += DeltaTime;
	// }
	// else
	// {
	// 	bThrowOnRelease = true;
	// 		
	// 	if (TargetDistanceFromView != ThrowingZoomLevel)
	// 	{
	// 		TargetDistanceFromView += ToThrowingZoomSpeed * (ThrowingZoomLevel - TargetDistanceFromView) *0.001;
	// 	}
	// 		
	// 	if (PrePrimingThrowTimer <= 1.0)
	// 	{
	// 		ThrowingTimeLine += FMath::Clamp(DeltaTime/ThrowChargeTime, 0.0,1.0);
	// 	}
	// }
}

/** The update loop that scales the zoomaxis value from the mouse input */
void UBHPlayerGrabComponent::UpdateZoomAxisValue(float ZoomAxis)
{
	// if(!bRotatingActor)
	// {
	// 	ControlZoomValue = FMath::Clamp(((ControlZoomValue + 0.1 * ZoomAxis) * 0.9),-2.0,2.0);
	// }
	// else
	// {
	// 	CurrentRotationZoomAxisValue = ZoomAxis;
	// }
	
}

void UBHPlayerGrabComponent::PrimeThrow()
{
	// bPrimingThrow = true;
	// PrePrimingThrowTimer = 0.0;
	// ThrowingTimeLine = 0.0f;
	// UE_LOG(LogGrabComponent, VeryVerbose, TEXT("Started Priming Throw."))
}

void UBHPlayerGrabComponent::StopPrimingThrow()
{
	// bPrimingThrow = false;
	// bThrowOnRelease = false;
	// PrePrimingThrowTimer = 0.0;
	// ThrowingTimeLine = 0.0f;
	// UE_LOG(LogGrabComponent, VeryVerbose, TEXT("Stopped Priming Throw."))
}

/** Execute a throw if the throw is priming*/
void UBHPlayerGrabComponent::Throw(bool bOnlyPreviewTrajectory)
{
	// if (!bThrowOnRelease) { return; }
	//
	// const UCameraComponent* Camera = GetPlayerCharacter()->GetCamera();
	// 	
	// bool ThrowOverHands = false;
	//
	// /** Calculate the throwing strenght using the timeline we updated in the tick.*/
	// const float ThrowingStrength = ThrowStrengthCurve->GetFloatValue(ThrowingTimeLine);
	//
	// FVector Target = FVector();
	// const FVector TraceStart = Camera->GetComponentLocation();
	// const FVector TraceEnd = Camera->GetForwardVector() * 10000000 + TraceStart;FHitResult HitResult;
	//
	// FCollisionQueryParams CollisionParams;
	// CollisionParams.AddIgnoredComponent(GrabbedComponent);
	// if  (this->GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, CollisionParams))
	// {
	// 	Target = HitResult.ImpactPoint;
	// }
	// else
	// {
	// 	Target = TraceEnd;
	// }
	// 	
	// /** Calculate the direction from the player to the target */
	// FVector Direction = Target - GrabbedComponent->GetComponentLocation();
	// Direction.Normalize();
	// 	
	// FVector FinalDirection = 0,0,0;
	// if(Target != TraceEnd)
	// {
	// 	/** Calculate the angle to throw the object using a ballistic trajectory.*/
	// 	//float ThrowAngle = CalculateThrowAngle(ReleaseLocation,Target,ThrowingStrength,ThrowOverHands);
	// 		
	// 	/**Rotate the Direction vector around the cross product of the Direction vector and the world up vector by the angle Theta. */
	// 	FVector RotationAxis = Direction.CrossProduct(Direction,FVector::UpVector);
	// 	RotationAxis.Normalize();
	// 	//FinalDirection = Direction.RotateAngleAxis(ThrowAngle, RotationAxis);
	// 	FinalDirection.Normalize();
	//
	// 	float VectorLength = RotationAxis.Size();
	// }
	// else
	// {
	// 	FinalDirection = Direction;
	// }
	//
	// /** for now, we won't use the manual calculation for throwing angle if there isn't any solution*/
	// FinalDirection = Direction;
	// 	
	// ThrowVelocity = FinalDirection * ThrowingStrength;
	//
	// FVector TossVelocity;
	// if(!UGameplayStatics::SuggestProjectileVelocity(
	// 		this,
	// 		TossVelocity,
	// 		GrabbedComponent->GetComponentLocation(),
	// 		Target,
	// 		ThrowingStrength,
	// 		false,
	// 		0,
	// 		GetWorld()->GetGravityZ(),
	// 		ESuggestProjVelocityTraceOption::DoNotTrace,
	// 		FCollisionResponseParams::DefaultResponseParam,
	// 		TArray<AActor*>= GetOwner(),GetGrabbedActor(),
	// 		false))
	// {
	// 	TossVelocity = ThrowVelocity;
	// }
	// VisualizeProjectilePath(GrabbedComponent->GetOwner(),GrabbedComponent->GetComponentLocation(),TossVelocity);
	//
	// if(!bOnlyPreviewTrajectory)
	// {
	// 	GrabbedComponent->WakeRigidBody();
	// 	ReleaseActor();
	// 	GrabbedComponent->SetPhysicsLinearVelocity(TossVelocity);
	// }
}

void UBHPlayerGrabComponent::VisualizeProjectilePath(AActor* ProjectileActor, FVector StartLocation, FVector LaunchVelocity)
{
	// /** Define the parameters for the prediction */
	// TArray<FVector> OutPathPositions;
	// FPredictProjectilePathResult OutPath;
	// FPredictProjectilePathParams PredictParams{};
	// PredictParams.StartLocation = StartLocation;
	// PredictParams.LaunchVelocity = LaunchVelocity;
	// PredictParams.bTraceComplex = false;
	// PredictParams.bTraceWithCollision = true;
	// PredictParams.DrawDebugType = EDrawDebugTrace::None;
	// PredictParams.DrawDebugTime = 2.0f;
	// PredictParams.SimFrequency = 30.0f;
	// PredictParams.MaxSimTime = 2.0f;
	// PredictParams.ActorsToIgnore = TArray<AActor*> = GetOwner(),GrabbedComponent->GetOwner();
	//
	// /** Call the prediction function */
	// UGameplayStatics::PredictProjectilePath(ProjectileActor,PredictParams,OutPath);
	//
	// if (OutPath.HitResult.bBlockingHit)
	// {
	// 	FVector HitLocation = OutPath.HitResult.Location;
	// 	float SphereRadius = 10.0f;
	// 	FColor SphereColor = FColor::Red;
	// 	float SphereLifeTime = 0.0f;
	// 	DrawDebugSphere(GetWorld(), HitLocation, SphereRadius, 32, SphereColor, false, SphereLifeTime);
	// }
	//
	//
	// /** for (int32 i = 0; i < OutPathPositions.Num(); ++i)
	// {
	// 	const FVector& PathPosition = OutPathPositions[i];
	// 	const FPredictProjectilePathPointData& PathPointData = OutPath.PathData[i];
	// } */
}