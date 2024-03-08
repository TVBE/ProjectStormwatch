// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHPlayerInteractionComponent.h"

#include "BHInteractableObjectInterface.h"
#include "BHPlayerCameraComponent.h"
#include "..\..\..\Interfaces\Public\BHDraggableObjectInterface.h"
#include "..\..\..\Interfaces\Public\BHGrabbableObjectInterface.h"
#include "..\..\..\Interfaces\Public\BHInventoryObjectInterface.h"
#include "..\..\..\Interfaces\Public\BHUsableObjectInterface.h"
#include "BHPlayerCharacter.h"
#include "BHPlayerController.h"
#include "BHPlayerDragComponent.h"
#include "BHPlayerInventoryComponent.h"
#include "BHPlayerGrabComponent.h"
#include "BHPlayerUseComponent.h"
#include "Runtime/Engine/Classes/Engine/EngineTypes.h"
#include "Camera/CameraComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogBHInteractionComponent, Log, All);

UBHPlayerInteractionComponent::UBHPlayerInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UBHPlayerInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ABHPlayerCharacter* Character = nullptr;
	if (const ABHPlayerController* PlayerController = Cast<ABHPlayerController>(GetOwner()))
	{
		Character = Cast<ABHPlayerCharacter>(PlayerController->GetPawn());
	}
	if (!IsValid(Character))
	{
		return;
	}
	
	const FBHCameraQuery& CameraQuery = Character->GetCamera()->GetCameraQuery();
	// Exit early if trace length exceeds maximum possible interaction distance,
	// as we will never be able to interact with something at this point.
	if (CameraQuery.TraceLength > MaxInteractionDistance + InteractionTraceRadius)
	{
		return;
	}

	const FVector& TraceLocation = CameraQuery.HitResult.ImpactPoint;
	TArray<TPair<UObject*, FVector>> InteractableObjects = TraceForInteractableObjects(TraceLocation, {Character});
	if (InteractableObjects.IsEmpty())
	{
		return;
	}

	UObject* PreferredInteractableObject = nullptr;
	for (TPair<UObject*, FVector> InteractableObject : InteractableObjects)
	{
		UObject* Object = InteractableObject.Key;
		const FVector& ObjectLocation = InteractableObject.Value;

		if (!IsOccluded(Object, TraceLocation, ObjectLocation))
		{
			PreferredInteractableObject = Object;
			break;
		}
	}

	
}

TArray<TPair<UObject*, FVector>> UBHPlayerInteractionComponent::TraceForInteractableObjects(const FVector& Location,
	const TArray<AActor*>& IgnoredActors)
{
	TArray<TPair<UObject*, FVector>> FoundObjects;

	TArray<FHitResult> HitResults;
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = false;
	QueryParams.bReturnPhysicalMaterial = false;
	QueryParams.AddIgnoredActors(IgnoredActors);
	const UWorld* World = GetWorld();
	if (const bool bHit = World ? World->SweepMultiByChannel(
		HitResults,
		Location,
		Location,
		FQuat::Identity,
		ECC_WorldDynamic,
		FCollisionShape::MakeSphere(InteractionTraceRadius),
		QueryParams) : false)
	{
		TArray<TTuple<UObject*, FVector, float, int32>> OverlappingObjects;
		for (const FHitResult& Hit : HitResults)
		{
			if (AActor* Actor = Hit.GetActor())
			{
				const float RadiusSquared = pow(InteractionTraceRadius, 2);
				if (float DistanceSquared = FVector::DistSquared(Actor->GetActorLocation(), Location);
					DistanceSquared <= RadiusSquared && Actor->Implements<UBHInteractableObject>())
				{
					int32 Priority = 0;
					if (bUsePriority)
					{
						Priority = IBHInteractableObject::Execute_GetInteractionPriority(Actor);
					}
					OverlappingObjects.Add(MakeTuple(Actor, Actor->GetActorLocation(), DistanceSquared, Priority));
				}
				TInlineComponentArray<UActorComponent*> Components(Actor);
				for (UActorComponent* Component : Components)
				{
					if (!IsValid(Component))
					{
						continue;
					}
					
					const USceneComponent* SceneComponent = Cast<USceneComponent>(Component);
					FVector ComponentLocation = SceneComponent ? SceneComponent->GetComponentLocation() : Actor->GetActorLocation();

					if (float DistanceSquared = FVector::DistSquared(ComponentLocation, Location);
						DistanceSquared <= RadiusSquared && Actor->Implements<UBHInteractableObject>())
					{
						int32 Priority = 0;
						if (bUsePriority)
						{
							Priority = IBHInteractableObject::Execute_GetInteractionPriority(Actor);
						}
						OverlappingObjects.Add(MakeTuple(Component, ComponentLocation, DistanceSquared, Priority));
					}
				}
			}
		}
		
		if (!OverlappingObjects.IsEmpty())
		{
			// Sort by priority first. If priorities are equal, sort by distance.
			Algo::Sort(OverlappingObjects,
				[](const TTuple<UObject*, float, int32>& A, const TTuple<UObject*, float, int32>& B) {
					if (A.Get<3>() != B.Get<3>()) {
						
						return A.Get<3>() < B.Get<3>();
					}
					return A.Get<2>() < B.Get<2>();
				}
			);
		}

		FoundObjects.Reserve(OverlappingObjects.Num());
		for (const auto& OverlappingObject : OverlappingObjects) {
			FoundObjects.Add(MakeTuple(OverlappingObject.Get<0>(), OverlappingObject.Get<1>()));
		}
	}
	return FoundObjects;
}

bool UBHPlayerInteractionComponent::IsOccluded(UObject* Object, const FVector& Start, const FVector& End)
{
	check(Object);
	
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	if (AActor* Actor = Cast<AActor>(Object))
	{
		CollisionParams.AddIgnoredActor(Actor);
	}
	else if (UActorComponent* Component = Cast<UActorComponent>(Object))
	{
		CollisionParams.AddIgnoredActor(Component->GetOwner());
	}
	const UWorld* World = Object->GetWorld();
	return World ? World->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams) : false;
}

AActor* UBHPlayerInteractionComponent::CheckForInteractableActor()
{

	const UCameraComponent* Camera = GetPlayerCharacter()->GetCamera();

	const FVector CameraLocation = Camera->GetComponentLocation();
	
	/** We reset the camera trace hit result instead of constructing a new one every check to prevent unnecessary memory allocation every frame. */
	CameraTraceHitResult.Reset(0, false);
	
	PerformTraceFromCamera(CameraTraceHitResult);

	/** Break off the execution and return a nullptr if the camera trace did not yield a valid blocking hit. */
	if (!CameraTraceHitResult.IsValidBlockingHit()) { return nullptr; }

	/** If the object the camera is looking at directly responds to the interactable collision channel, return that actor. */
	if (AActor* HitActor = CameraTraceHitResult.GetActor(); HitActor &&
		HitActor->GetRootComponent()->GetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1) == ECollisionResponse::ECR_Block)
	{
		/** Check if the object is small. Large objects like tables should be ignored. */
		const FBox BoundingBox = HitActor->GetComponentsBoundingBox(true);
		const FVector BoxExtent = BoundingBox.GetExtent();
		
		if (const float BoundingBoxVolume = static_cast<float>(BoxExtent.X * BoxExtent.Y * BoxExtent.Z); BoundingBoxVolume < 2000.0f)
		{
			return HitActor;
		}
	}

	/** Perform a multi sphere sweep for interactable object and get the actor closest to the camera trace hit. */
	ObjectTraceHitResults.Empty();
	PerformInteractableObjectTrace(ObjectTraceHitResults, CameraTraceHitResult);
	if (ObjectTraceHitResults.IsEmpty()) { return nullptr; }
	AActor* ClosestActor = GetClosestActor(ObjectTraceHitResults, CameraTraceHitResult);

	/** We reset the occlusion trace hit result instead of constructing a new one every check to prevent unnecessary memory allocation every frame. */
	OcclusionTraceHitResult.Reset(0, false);
	if (IsActorOccluded(ClosestActor)) { ClosestActor = nullptr; }
	
	return ClosestActor;
}

/** Performs a line trace in the direction of the camera's forward vector. */
void UBHPlayerInteractionComponent::PerformTraceFromCamera(FHitResult& HitResult)
{
	const UCameraComponent* Camera = GetPlayerCharacter()->GetCamera();

	const FVector CameraLocation = Camera->GetComponentLocation();

	const FVector EndLocation = CameraLocation + Camera->GetForwardVector() * MaxInteractionDistance;
	
	GetWorld()->LineTraceSingleByChannel(
		HitResult,
		CameraLocation,
		EndLocation,
		ECollisionChannel::ECC_Visibility,
		CameraTraceQueryParams
	);

#if WITH_EDITORONLY_DATA
	if (bDebugVisEnabled)
	{
		DrawDebugLine(GetWorld(), CameraLocation, EndLocation, FColor::White, false, 0.0f, 0, 3.0f);
	}
#endif
}

/** Performs a multi sphere trace at the hit location of a hit result and populates and array of hit results. */
void UBHPlayerInteractionComponent::PerformInteractableObjectTrace(TArray<FHitResult>& Array, const FHitResult& HitResult)
{
	if (!GetWorld()) { return; }

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());

	GetWorld()->SweepMultiByChannel(
		Array,
		HitResult.ImpactPoint,
		HitResult.ImpactPoint,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel1,
		FCollisionShape::MakeSphere(ObjectTraceRadius),
		QueryParams
	);

#if WITH_EDITORONLY_DATA
	if (bDebugVisEnabled)
	{
		DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, ObjectTraceRadius, 32, FColor::White, false, 0.0f, 0, 2.0f);
	}
#endif
}

/** Returns the actor closest to the hit location of a hit result. */
AActor* UBHPlayerInteractionComponent::GetClosestActor(const TArray<FHitResult>& Array, const FHitResult& HitResult)
{
	AActor* ClosestActor = nullptr;
	float MinDistance = MAX_FLT;
	
	for (const FHitResult& ObjectHitResult : Array)
	{
		const float CurrentDistance = FVector::DistSquared(ObjectHitResult.GetActor()->GetActorLocation(), HitResult.ImpactPoint);
		if (CurrentDistance < MinDistance)
		{
			MinDistance = CurrentDistance;
			ClosestActor = ObjectHitResult.GetActor();
		}
	}
	return ClosestActor;
}

template <typename TInterface>
void UBHPlayerInteractionComponent::AddInteractableObjectsOfType(AActor* Actor, EBHInteractionType InteractionType)
{
	TArray<UObject*> InteractableObjects {FindInteractableObjects<TInterface>(Actor)};
	if (!InteractableObjects.IsEmpty())
	{
		for (UObject* InteractableObject : InteractableObjects)
		{
			CurrentInteractableObjects.Add(FBHInteractableObjectData(InteractableObject, InteractionType));
		}
	}
}

void UBHPlayerInteractionComponent::UpdateInteractableObjectData(AActor* NewInteractableActor)
{
	if (!NewInteractableActor) { return; }

	CurrentInteractableObjects.Empty();

	AddInteractableObjectsOfType<UBHUsableObject>(NewInteractableActor, EBHInteractionType::Usable);
	AddInteractableObjectsOfType<UBHGrabbableObject>(NewInteractableActor, EBHInteractionType::Grabbable);
	AddInteractableObjectsOfType<UBHDraggableObject>(NewInteractableActor, EBHInteractionType::Draggable);
}

bool UBHPlayerInteractionComponent::IsActorOccluded(const AActor* Actor)
{
	if (!Actor)
	{
		
		return false;
	}

	const UCameraComponent* Camera = GetPlayerCharacter()->GetCamera();

	const FVector CameraLocation = Camera->GetComponentLocation();
	
	const FVector EndLocation = Actor->GetActorLocation();
	FCollisionQueryParams QueryParams = FCollisionQueryParams(FName(TEXT("VisibilityTrace")), false, nullptr);
	QueryParams.AddIgnoredActor(GetOwner());
	
	const bool bHit = GetWorld()->LineTraceSingleByChannel(
		OcclusionTraceHitResult,
		CameraLocation,
		EndLocation + OcclusionOffset,
		ECollisionChannel::ECC_Visibility,
		QueryParams
	);

	return bHit && OcclusionTraceHitResult.GetActor() && (OcclusionTraceHitResult.GetActor() != Actor);
}


inline FVector GetNearestPointOnMesh(const FHitResult& HitResult, const AActor* Actor)
{
	FVector TargetLocation = FVector();
	
	if (!Actor) { return TargetLocation; }
	
	if (const UStaticMeshComponent* MeshComponent = Cast<UStaticMeshComponent>(Actor->FindComponentByClass(UStaticMeshComponent::StaticClass())); HitResult.IsValidBlockingHit())
	{
		const FVector MeshCenter = MeshComponent->Bounds.Origin;
		const FVector Start = HitResult.ImpactPoint;
		const FVector End = MeshCenter;

		TArray<FHitResult> HitResults;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(HitResult.GetActor());
		
		const UWorld* World = MeshComponent->GetWorld();
		
		if (bool bHit = World->LineTraceMultiByChannel(HitResults, Start, End, ECC_Visibility, CollisionParams))
		{
			for (const FHitResult& MultiHitResult : HitResults)
			{
				if (MultiHitResult.GetComponent() == MeshComponent)
				{
					TargetLocation = MultiHitResult.ImpactPoint;
					break;
				}
			}
		}
	}
	return TargetLocation;
}

