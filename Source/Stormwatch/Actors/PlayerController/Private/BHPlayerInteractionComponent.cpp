// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHPlayerInteractionComponent.h"

#include "BHDraggableObjectInterface.h"
#include "BHGrabbableObjectInterface.h"
#include "BHInteractableObjectInterface.h"
#include "BHPlayerCameraComponent.h"
#include "BHPlayerCharacter.h"
#include "BHPlayerController.h"
#include "BHPlayerDragComponent.h"
#include "BHPlayerGrabComponent.h"
#include "BHPlayerUseComponent.h"
#include "BHUsableObjectInterface.h"

#include "Runtime/Engine/Classes/Engine/EngineTypes.h"

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

	if (!Character->IsInteracting())
	{
		const FBHCameraQuery& CameraQuery = Character->GetCamera()->GetCameraQuery();
		// Exit early if trace length exceeds maximum possible interaction distance,
		// as we will never be able to interact with something at this point.
		if (CameraQuery.HitResult.IsValidBlockingHit() &&
			CameraQuery.TraceLength > MaxInteractionDistance + InteractionTraceRadius)
		{
			return;
		}

		const FVector& TraceLocation = CameraQuery.HitResult.ImpactPoint;
		TArray<TPair<UObject*, FVector>> InteractableObjects = TraceForInteractableObjects(TraceLocation, {Character});
		if (InteractableObjects.IsEmpty())
		{
			return;
		}

		TOptional<UObject*> PreferredInteractableObject;
		for (TPair<UObject*, FVector> InteractableObject : InteractableObjects)
		{
			UObject* Object = InteractableObject.Key;
			const FVector& ObjectLocation = InteractableObject.Value;

			if (!IsOccluded(Object, TraceLocation, ObjectLocation))
			{
				if (!PreferredInteractableObject.IsSet())
				{
					PreferredInteractableObject.Emplace(Object);
				}
#if WITH_EDITORONLY_DATA
				if (bEnableDebugVisualisation)
				{
					DrawDebugSphere(GetWorld(), ObjectLocation, 20, 12, FColor::Green, false, 0.0f, 0, 2.0f);
					continue;
				}
#endif
				break;
			}
		}
		if (!PreferredInteractableObject.IsSet())
		{
			return;
		}
		if (CurrentInteractableObject.IsSet() &&
			CurrentInteractableObject.GetValue().Object == PreferredInteractableObject.GetValue())
		{
			return;
		}
		
		const EBHInteractionType InteractionType = DetermineInteractionType(PreferredInteractableObject.GetValue());
		CurrentInteractableObject.Emplace(FBHInteractionObject(PreferredInteractableObject.GetValue(), InteractionType));
		
		OnNewInteractableObjectFound.Broadcast(PreferredInteractableObject.GetValue(), InteractionType);

		UE_LOG(LogBHInteractionComponent, Verbose, TEXT("Found new interactable object: [%s] [Class: %s, Type: %s]"),
				*PreferredInteractableObject.GetValue()->GetName(),
				*PreferredInteractableObject.GetValue()->GetClass()->GetName(),
				*UEnum::GetValueAsString(InteractionType));
	}
}

bool UBHPlayerInteractionComponent::IsCharacterInteracting() const
{
	const ABHPlayerController* Controller = Cast<ABHPlayerController>(GetOwner());
	if (!IsValid(Controller))
	{
		return false;
	}

	const ABHPlayerCharacter* Character = Controller->GetControlledCharacter();
	if (!IsValid(Character))
	{
		UE_LOG(LogBHInteractionComponent, Warning,
			TEXT("IsCharacterInteracting: Controller [%s] is currently not controlling a character."),
			*Controller->GetName());
		return false;
	}
	
	return Character->GetGrabComponent()->IsHoldingObject() ||
		   Character->GetDragComponent()->IsHoldingObject() ||
		   Character->GetUseComponent()->IsUsingObject();
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
#if WITH_EDITORONLY_DATA
		if (bEnableDebugVisualisation)
		{
			DrawDebugSphere(GetWorld(), Location, InteractionTraceRadius, 32, FColor::White, false, 0.0f, 0, 2.0f);
		}
#endif
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
						DistanceSquared <= RadiusSquared && Component->Implements<UBHInteractableObject>())
					{
						int32 Priority = 0;
						if (bUsePriority)
						{
							Priority = IBHInteractableObject::Execute_GetInteractionPriority(Component);
						}
						OverlappingObjects.Add(MakeTuple(Component, ComponentLocation, DistanceSquared, Priority));
					}
				}
			}
		}
		
		if (!OverlappingObjects.IsEmpty())
		{
			Algo::Sort(OverlappingObjects,
				[this](const TTuple<UObject*, FVector, float, int32>& A, const TTuple<UObject*, FVector, float, int32>& B) {
					if (!bUsePriorityDistanceThreshold || A.Get<2>() < PriorityDistanceThreshold || B.Get<2>() > PriorityDistanceThreshold)
					{
						return A.Get<2>() < B.Get<2>();
					}
					if (A.Get<3>() != B.Get<3>())
					{
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
	bool bHit = World ? World->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams) : false;

#if WITH_EDITOR
	if (bEnableDebugVisualisation)
	{
		const FColor LineColor = bHit ? FColor::White : FColor::Red;
		DrawDebugLine(GetWorld(), Start, End, FColor::White, false, 0.0f, 0, 3.0f);
	}
#endif

	return bHit;
}

EBHInteractionType UBHPlayerInteractionComponent::DetermineInteractionType(const UObject* Object)
{
	if (ensureAlways(IsValid(Object)))
	{
		if (const UClass* Class = Object->GetClass())
		{
			if (Class->ImplementsInterface(UBHGrabbableObject::StaticClass()))
			{
				return EBHInteractionType::Grabbable;
			}
			if (Class->ImplementsInterface(UBHDraggableObject::StaticClass()))
			{
				return EBHInteractionType::Draggable;
			}
			if (Class->ImplementsInterface(UBHUsableObject::StaticClass()))
			{
				return EBHInteractionType::Usable;
			}
		}
	}
	return EBHInteractionType::None;
}

FVector UBHPlayerInteractionComponent::GetNearestPointOnMesh(const FHitResult& HitResult, const AActor* Actor)
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



