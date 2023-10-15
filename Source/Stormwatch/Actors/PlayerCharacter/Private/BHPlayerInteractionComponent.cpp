// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHPlayerInteractionComponent.h"
#include "..\..\..\Interfaces\Public\BHDraggableObjectInterface.h"
#include "..\..\..\Interfaces\Public\BHGrabbableObjectInterface.h"
#include "..\..\..\Interfaces\Public\BHInventoryObjectInterface.h"
#include "..\..\..\Interfaces\Public\BHUsableObjectInterface.h"
#include "BHPlayerCharacter.h"
#include "BHPlayerDragComponent.h"
#include "BHPlayerInventoryComponent.h"
#include "BHPlayerGrabComponent.h"
#include "BHPlayerUseComponent.h"
#include "Runtime/Engine/Classes/Engine/EngineTypes.h"
#include "Camera/CameraComponent.h"

UBHPlayerInteractionComponent::UBHPlayerInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UBHPlayerInteractionComponent::OnRegister()
{
	Super::OnRegister();
	
	CameraTraceQueryParams = FCollisionQueryParams(FName(TEXT("VisibilityTrace")), false, GetOwner());
	CameraTraceQueryParams.bReturnPhysicalMaterial = false;
}

void UBHPlayerInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBHPlayerInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UBHPlayerGrabComponent* GrabComponent = GetPlayerCharacter()->GetGrabComponent();
	UBHPlayerUseComponent* UseComponent = GetPlayerCharacter()->GetUseComponent();

	if (GrabComponent && GrabComponent->GetGrabbedActor())
	{
		CurrentInteractableActor = nullptr;
		return;
	}
	
	if (AActor* InteractableActor = CheckForInteractableActor())
	{
		if (InteractableActor != CurrentInteractableActor)
		{
			CurrentInteractableActor = InteractableActor;
			UpdateInteractableObjectData(InteractableActor);
		}
	}
	else 
	{
		if (!CurrentInteractableObjects.IsEmpty())
		{
			CurrentInteractableObjects.Empty();
		}
		CurrentInteractableActor = nullptr;
	}

	GetClosestObjectToLocation(ClosestInteractableObject, CameraTraceHitResult.Location, CurrentInteractableObjects);
	
	if (UseComponent && UseComponent->GetActorInUse() != CurrentInteractableActor)
	{
		UseComponent->EndUse();
	}
	
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

	const FVector EndLocation = CameraLocation + Camera->GetForwardVector() * CameraTraceLength;
	
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
	if (!Actor) { return false; }

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

template <typename TInterface>
UObject* UBHPlayerInteractionComponent::FindInteractableObject(AActor* Actor) const
{
	if (!Actor) { return nullptr; }
	UObject* InteractableObject = nullptr;
	
	/** Check if the actor implements the specified interface. */
	if (Actor->GetClass()->ImplementsInterface(TInterface::StaticClass()))
	{
		InteractableObject = Actor;
	}
	
	/** If the actor does not implement the specified interface, try to find a component that does.*/
	else if (UActorComponent* InteractableComponent = FindInteractableComponent<TInterface>(Actor))
	{
		InteractableObject = InteractableComponent;
	}
	
	return InteractableObject;
}

template <typename TInterface>
TArray<UObject*> UBHPlayerInteractionComponent::FindInteractableObjects(AActor* Actor) const
{
	TArray<UObject*> InteractableObjects;

	if (!Actor) { return InteractableObjects; }

	/** Check if the actor implements the specified interface. */
	if (Actor->GetClass()->ImplementsInterface(TInterface::StaticClass()))
	{
		InteractableObjects.Add(Actor);
	}

	/** Iterate through the actor's components, checking if any of them implement the specified interface. */
	TArray<UActorComponent*> Components;
	Actor->GetComponents(Components);
	for (UActorComponent* Component : Components)
	{
		if (Component->GetClass()->ImplementsInterface(TInterface::StaticClass()))
		{
			InteractableObjects.Add(Component);
		}
	}

	return InteractableObjects;
}

template <typename TInterface>
UActorComponent* UBHPlayerInteractionComponent::FindInteractableComponent(const AActor* Actor) const
{
	if (!Actor) { return nullptr; }
	TSet<UActorComponent*> Components {Actor->GetComponents()};
	if (Components.IsEmpty()) { return nullptr; }
	for (UActorComponent* Component : Components)
	{
		if (Component->GetClass()->ImplementsInterface(TInterface::StaticClass()))
		{
			return Component;
		}
	}
	return nullptr;
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

AActor* UBHPlayerInteractionComponent::GetActorFromObject(UObject* Object) const
{
	if (AActor* Actor = Cast<AActor>(Object))
	{
		return Actor;
	}
	
	if (const UActorComponent* Component = Cast<UActorComponent>(Object))
	{
		return Component->GetOwner();
	}
	return nullptr;
}

bool UBHPlayerInteractionComponent::GetClosestObjectToLocation(FBHInteractableObjectData& OutInteractableObjectData, const FVector& Location, TArray<FBHInteractableObjectData> Objects)
{
	if (Objects.IsEmpty())
	{
		OutInteractableObjectData.Object = nullptr;
		return false;
	}

	const UObject* ClosestObject = nullptr;
	float MinDistance = FLT_MAX;
	FBHInteractableObjectData ClosestObjectData;

	for (const FBHInteractableObjectData& ObjectData : Objects)
	{
		UObject* Object = ObjectData.Object;
		FVector ObjectLocation;

		if (const AActor* Actor = Cast<AActor>(Object))
		{
			ObjectLocation = Actor->GetActorLocation();
		}
		else if (const USceneComponent* Component = Cast<USceneComponent>(Object))
		{
			ObjectLocation = Component->GetComponentLocation();
		}
		else
		{
			continue;
		}

		if (const float Distance = static_cast<float>(FVector::DistSquared(Location, ObjectLocation)); Distance < MinDistance)
		{
			MinDistance = Distance;
			ClosestObject = Object;
			ClosestObjectData = ObjectData;
		}
	}

	if (ClosestObject)
	{
		OutInteractableObjectData = ClosestObjectData;
		return true;
	}

	return false;
}

void UBHPlayerInteractionComponent::BeginPrimaryInteraction()
{
	UBHPlayerUseComponent* UseComponent = GetPlayerCharacter()->GetUseComponent();

	if (CurrentInteractableActor)
	{
		if (UObject* InteractableObject = FindInteractableObject<UBHUsableObject>(CurrentInteractableActor))
		{
			UseComponent->BeginUse(InteractableObject);
		}
	}
}

void UBHPlayerInteractionComponent::EndPrimaryInteraction()
{
	UBHPlayerUseComponent* UseComponent = GetPlayerCharacter()->GetUseComponent();

	if (!UseComponent->GetObjectInUse()) { return; }

	UseComponent->EndUse();
}

void UBHPlayerInteractionComponent::BeginSecondaryInteraction()
{
	UBHPlayerGrabComponent* GrabComponent = GetPlayerCharacter()->GetGrabComponent();
	UBHPlayerDragComponent* DragComponent = GetPlayerCharacter()->GetDragComponent();

	if (GrabComponent->GetGrabbedActor())
	{
		GrabComponent->PrimeThrow();
		return;
	}

	if (!CurrentInteractableActor) { return; }
	
	if (UObject* GrabbableObject = FindInteractableObject<UBHGrabbableObject>(CurrentInteractableActor))
	{
		GrabComponent->GrabActor(CurrentInteractableActor);
	}
	else if (UObject* DraggableObject = FindInteractableObject<UBHDraggableObject>(CurrentInteractableActor))
	{
		FVector GrabLocation = FVector();
		if (CameraTraceHitResult.GetActor() == CurrentInteractableActor)
		{
			GrabLocation = CameraTraceHitResult.ImpactPoint;
		}
		else if (CameraTraceHitResult.GetActor() != CurrentInteractableActor)
		{
			GrabLocation = GetNearestPointOnMesh(CameraTraceHitResult, CurrentInteractableActor);
		}
		DragComponent->DragActorAtLocation(CurrentInteractableActor, GrabLocation);
	}
}

void UBHPlayerInteractionComponent::EndSecondaryInteraction()
{
	UBHPlayerGrabComponent* GrabComponent = GetPlayerCharacter()->GetGrabComponent();
	UBHPlayerDragComponent* DragComponent = GetPlayerCharacter()->GetDragComponent();

	if (GrabComponent->GetWillThrowOnRelease())
	{
		GrabComponent->PerformThrow(false);
	}
	else
	{
		if(GrabComponent->GetIsPrimingThrow())
		{
			GrabComponent->ReleaseActor();
		}
	}

	DragComponent->ReleaseActor();
}

void UBHPlayerInteractionComponent::BeginTertiaryInteraction()
{
	UBHPlayerGrabComponent* GrabComponent = GetPlayerCharacter()->GetGrabComponent();

	bTertiaryInteractionActive = true;

	GrabComponent->BeginTetriaryInteraction();
}

void UBHPlayerInteractionComponent::EndTertiaryInteraction()
{
	UBHPlayerGrabComponent* GrabComponent = GetPlayerCharacter()->GetGrabComponent();

	bTertiaryInteractionActive = false;

	GrabComponent->EndTetriaryInteraction();
}

void UBHPlayerInteractionComponent::BeginInventoryInteraction()
{
	UBHPlayerInventoryComponent* InventoryComponent = GetPlayerCharacter()->GetInventoryComponent();

	if (CurrentInteractableActor)
	{
		if (UObject* InteractableObject = FindInteractableObject<UBHInventoryObject>(CurrentInteractableActor))
		{
			InventoryComponent->AddActorToInventory(GetActorFromObject(CurrentInteractableActor));
		}
	}
}

void UBHPlayerInteractionComponent::EndInventoryInteraction()
{
}

void UBHPlayerInteractionComponent::AddScrollInput(const float Input)
{
	UBHPlayerGrabComponent* GrabComponent = GetPlayerCharacter()->GetGrabComponent();

	if (GrabComponent->GetGrabbedActor())
	{
		GrabComponent->UpdateZoomAxisValue(Input);
	}
}

void UBHPlayerInteractionComponent::AddPitchInput(const float Input)
{
	UBHPlayerGrabComponent* GrabComponent = GetPlayerCharacter()->GetGrabComponent();

	if (bTertiaryInteractionActive && GrabComponent->GetGrabbedActor())
	{
		GrabComponent->UpdateMouseInputRotation(FVector2D(0, Input));
	}
}

void UBHPlayerInteractionComponent::AddYawInput(const float Input)
{
	UBHPlayerGrabComponent* GrabComponent = GetPlayerCharacter()->GetGrabComponent();

	if (bTertiaryInteractionActive && GrabComponent->GetGrabbedActor())
	{
		GrabComponent->UpdateMouseInputRotation(FVector2D(Input, 0));
	}
}

void UBHPlayerInteractionComponent::OnUnregister()
{
	Super::OnUnregister();
}

void UBHPlayerInteractionComponent::EventEndInteraction_Implementation(const EBHInteractionActionType Type, const UObject* Object)
{
}

void UBHPlayerInteractionComponent::EventBeginInteraction_Implementation(const EBHInteractionActionType Type, const UObject* Object)
{
}

