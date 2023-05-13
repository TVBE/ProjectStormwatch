// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne & Nino Saglia
// This source code is part of the project Frostbite

#include "PlayerInteractionComponent.h"

#include "DraggableObjectInterface.h"
#include "GrabbableObjectInterface.h"
#include "InventoryObjectInterface.h"
#include "UsableObjectInterface.h"
#include "PlayerCharacter.h"
#include "PlayerDragComponent.h"
#include "PlayerInventoryComponent.h"
#include "PlayerGrabComponent.h"
#include "PlayerUseComponent.h"
#include "Runtime/Engine/Classes/Engine/EngineTypes.h"
#include "Camera/CameraComponent.h"

UPlayerInteractionComponent::UPlayerInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UPlayerInteractionComponent::OnRegister()
{
	Super::OnRegister();
	
	if (const APlayerCharacter* PlayerCharacter {Cast<APlayerCharacter>(GetOwner())})
	{
		Camera = PlayerCharacter->GetCamera();
	}

	CameraTraceQueryParams = FCollisionQueryParams(FName(TEXT("VisibilityTrace")), false, GetOwner());
	CameraTraceQueryParams.bReturnPhysicalMaterial = false;

	/** Add the necessary components to the owner. */
	UseComponent = Cast<UPlayerUseComponent>(GetOwner()->AddComponentByClass(UPlayerUseComponent::StaticClass(), false, FTransform(), false));
	GrabComponent = Cast<UPlayerGrabComponent>(GetOwner()->AddComponentByClass(UPlayerGrabComponent::StaticClass(), false, FTransform(), false));
	DragComponent = Cast<UPlayerDragComponent>(GetOwner()->AddComponentByClass(UPlayerDragComponent::StaticClass(), false, FTransform(), false));
	
	if (GrabComponent)
	{
		GrabComponent->Configuration = PlayerPhysicsGrabConfiguration.LoadSynchronous();
	}
}

void UPlayerInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwner())
	{
		InventoryComponent = Cast<UPlayerInventoryComponent>(GetOwner()->FindComponentByClass(UPlayerInventoryComponent::StaticClass()));
	}
	if (DragComponent)
	{
		DragComponent->InteractionComponent = this;
	}
}

void UPlayerInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GrabComponent && GrabComponent->GetGrabbedActor())
	{
		CurrentInteractableActor = nullptr;
		return;
	}
	
	CurrentInteractableActor = CheckForInteractableActor();
	
	if (UseComponent && UseComponent->GetActorInUse() != CurrentInteractableActor)
	{
		UseComponent->EndUse();
	}

	/** Check if the current interactable actor is different from the previous interactable actor.
	 *	Call OnInteractableActorFound if this is the case.
	 *	This allows other objects to perform a check once a new interactable actor is found. */
	if (CurrentInteractableActor && CurrentInteractableActor != PreviousInteractableActor)
	{
		OnInteractableActorFound.Broadcast(CurrentInteractableActor);
	}
	
	PreviousInteractableActor = CurrentInteractableActor;
}

AActor* UPlayerInteractionComponent::CheckForInteractableActor()
{
	if (!Camera) { return nullptr; }
	
	CameraLocation = Camera->GetComponentLocation();
	
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
		const FBox BoundingBox {HitActor->GetComponentsBoundingBox(true)};
		const FVector BoxExtent {BoundingBox.GetExtent()};
		
		if (const float BoundingBoxVolume {static_cast<float>(BoxExtent.X * BoxExtent.Y * BoxExtent.Z)}; BoundingBoxVolume < 2000.0f)
		{
			return HitActor;
		}
	}

	/** Perform a multi sphere sweep for interactable object and get the actor closest to the camera trace hit. */
	ObjectTraceHitResults.Empty();
	PerformInteractableObjectTrace(ObjectTraceHitResults, CameraTraceHitResult);
	if (ObjectTraceHitResults.IsEmpty()) { return nullptr; }
	AActor* ClosestActor {GetClosestObject(ObjectTraceHitResults, CameraTraceHitResult)};

	/** We reset the occlusion trace hit result instead of constructing a new one every check to prevent unnecessary memory allocation every frame. */
	OcclusionTraceHitResult.Reset(0, false);
	if (IsActorOccluded(ClosestActor)) { ClosestActor = nullptr; }
	
	return ClosestActor;
}

/** Performs a line trace in the direction of the camera's forward vector. */
void UPlayerInteractionComponent::PerformTraceFromCamera(FHitResult& HitResult)
{
	const FVector EndLocation = CameraLocation + Camera->GetForwardVector() * CameraTraceLength;
	
	GetWorld()->LineTraceSingleByChannel(
		HitResult,
		CameraLocation,
		EndLocation,
		ECollisionChannel::ECC_Visibility,
		CameraTraceQueryParams
	);

	if (IsDebugVisEnabled)
	{
		DrawDebugLine(GetWorld(), CameraLocation, EndLocation, FColor::White, false, 0.0f, 0, 3.0f);
	}
}

/** Performs a multi sphere trace at the hit location of a hit result and populates and array of hit results. */
void UPlayerInteractionComponent::PerformInteractableObjectTrace(TArray<FHitResult>& Array, const FHitResult& HitResult)
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

	if (IsDebugVisEnabled)
	{
		DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, ObjectTraceRadius, 32, FColor::White, false, 0.0f, 0, 2.0f);
	}
}

/** Returns the actor closest to the hit location of a hit result. */
AActor* UPlayerInteractionComponent::GetClosestObject(const TArray<FHitResult>& Array, const FHitResult& HitResult)
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

bool UPlayerInteractionComponent::IsActorOccluded(const AActor* Actor)
{
	if (!Actor) { return false; }
	
	const FVector EndLocation = Actor->GetActorLocation();
	FCollisionQueryParams QueryParams = FCollisionQueryParams(FName(TEXT("VisibilityTrace")), false, nullptr);
	QueryParams.AddIgnoredActor(GetOwner());
	
	const bool IsHit = GetWorld()->LineTraceSingleByChannel(
		OcclusionTraceHitResult,
		CameraLocation,
		EndLocation + OcclusionOffset,
		ECollisionChannel::ECC_Visibility,
		QueryParams
	);

	/** If the line trace hits an object other than the target actor, we assume the target actor is occluded. */
	const bool IsOccluded = IsHit && OcclusionTraceHitResult.GetActor() && OcclusionTraceHitResult.GetActor() != Actor;
	return IsOccluded;
}

template <typename TInterface>
UObject* UPlayerInteractionComponent::FindInteractableObject(AActor* Actor) const
{
	if (!Actor) { return nullptr; }
	UObject* InteractableObject {nullptr};
	
	/** Check if the actor implements the specified interface. */
	if (Actor->GetClass()->ImplementsInterface(TInterface::StaticClass()))
	{
		InteractableObject = Actor;
	}
	
	/** If the actor does not implement the specified interface, try to find a component that does.*/
	else if (UActorComponent* InteractableComponent {FindInteractableComponent<TInterface>(Actor)})
	{
		InteractableObject = InteractableComponent;
	}
	
	return InteractableObject;
}

template <typename TInterface>
UActorComponent* UPlayerInteractionComponent::FindInteractableComponent(const AActor* Actor) const
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
	FVector TargetLocation {FVector()};
	
	if (!Actor) { return TargetLocation; }
	
	if (const UStaticMeshComponent* MeshComponent {Cast<UStaticMeshComponent>(Actor->FindComponentByClass(UStaticMeshComponent::StaticClass()))}; HitResult.IsValidBlockingHit())
	{
		const FVector MeshCenter {MeshComponent->Bounds.Origin};
		const FVector Start {HitResult.ImpactPoint};
		const FVector End {MeshCenter};

		TArray<FHitResult> HitResults;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(HitResult.GetActor());
		
		/** We treat the mesh component as our world context object. */
		const UWorld* World {MeshComponent->GetWorld()};
		
		if (bool IsHit = World->LineTraceMultiByChannel(HitResults, Start, End, ECC_Visibility, CollisionParams))
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

AActor* UPlayerInteractionComponent::GetActorFromObject(UObject* Object) const
{
	if (AActor* Actor {Cast<AActor>(Object)})
	{
		return Actor;
	}
	
	if (const UActorComponent* Component {Cast<UActorComponent>(Object)})
	{
		return Component->GetOwner();
	}
	return nullptr;
}

void UPlayerInteractionComponent::BeginPrimaryInteraction()
{
	if (CurrentInteractableActor && UseComponent)
	{
		if (UObject* InteractableObject {FindInteractableObject<UUsableObject>(CurrentInteractableActor)})
		{
			UseComponent->BeginUse(InteractableObject);
		}
	}
}

void UPlayerInteractionComponent::EndPrimaryInteraction()
{
	if (!UseComponent || !UseComponent->GetObjectInUse()) { return; }
	UseComponent->EndUse();
}

void UPlayerInteractionComponent::BeginSecondaryInteraction()
{
	if (GrabComponent && DragComponent)
	{
		if (GrabComponent->GetGrabbedActor())
		{
			GrabComponent->BeginPrimingThrow();
		}
		else if (CurrentInteractableActor)
		{
			if (UObject* GrabbableObject {FindInteractableObject<UGrabbableObject>(CurrentInteractableActor)})
			{
				GrabComponent->GrabActor(CurrentInteractableActor);
			}
			else if (UObject* DraggableObject {FindInteractableObject<UDraggableObject>(CurrentInteractableActor)})
			{
				FVector GrabLocation {FVector()};
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
	}
}

void UPlayerInteractionComponent::EndSecondaryInteraction()
{
	if (GrabComponent && GrabComponent->GetWillThrowOnRelease())
	{
		GrabComponent->PerformThrow(false);
	}
	else
	{
		if(GrabComponent->GetIsPrimingThrow())
		{
			GrabComponent->ReleaseObject();
		}
	}
	if(DragComponent)
	{
		DragComponent->ReleaseActor();
	}
}

void UPlayerInteractionComponent::BeginTertiaryInteraction()
{
	IsTertiaryInteractionActive = true;
	GrabComponent->BeginTetriaryInteraction();
}

void UPlayerInteractionComponent::EndTertiaryInteraction()
{
	IsTertiaryInteractionActive = false;
	GrabComponent->EndTetriaryInteraction();
}

void UPlayerInteractionComponent::BeginInventoryInteraction()
{
	if (CurrentInteractableActor && InventoryComponent)
	{
		if (UObject* InteractableObject {FindInteractableObject<UInventoryObject>(CurrentInteractableActor)})
		{
			InventoryComponent->AddActorToInventory(GetActorFromObject(CurrentInteractableActor));
		}
	}
}

void UPlayerInteractionComponent::EndInventoryInteraction()
{
}

void UPlayerInteractionComponent::AddScrollInput(const float Input)
{
	if (GrabComponent && GrabComponent->GetGrabbedActor())
	{
		GrabComponent->UpdateZoomAxisValue(Input);
	}
}

void UPlayerInteractionComponent::AddPitchInput(const float Input)
{
	if (IsTertiaryInteractionActive && GrabComponent && GrabComponent->GetGrabbedActor())
	{
		GrabComponent->UpdateMouseImputRotation(FVector2D(0, Input));
	}
}

void UPlayerInteractionComponent::AddYawInput(const float Input)
{
	if (IsTertiaryInteractionActive && GrabComponent && GrabComponent->GetGrabbedActor())
	{
		GrabComponent->UpdateMouseImputRotation(FVector2D(Input, 0));
	}
}

void UPlayerInteractionComponent::OnUnregister()
{
	Super::OnUnregister();
}

void UPlayerInteractionComponent::EventEndInteraction_Implementation(const EInteractionActionType Type, const UObject* Object)
{
}

void UPlayerInteractionComponent::EventBeginInteraction_Implementation(const EInteractionActionType Type, const UObject* Object)
{
}

