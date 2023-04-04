// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne & Nino Saglia
// This source code is part of the project Frostbite

#include "PlayerInteractionComponent.h"
#include "InteractableObjectInterface.h"
#include "PlayerCharacter.h"
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
	
	GrabComponent = Cast<UPlayerPhysicsGrabComponent>(GetOwner()->AddComponentByClass(UPlayerPhysicsGrabComponent::StaticClass(), false, FTransform(), false));

	if (GrabComponent)
	{
		GrabComponent->Configuration = PlayerPhysicsGrabConfiguration.LoadSynchronous();
		if (GrabComponent->Configuration)
		{
			GrabComponent->Configuration->ApplyToPhysicsHandle(GrabComponent);
		}
		
	}
}

/** Called when the game starts. */
void UPlayerInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
}

/** Called every frame. */
void UPlayerInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GrabComponent && GrabComponent->GetGrabbedActor()) { return; }
	
	CurrentInteractableActor = CheckForInteractableActor();
	
	if (ActorInUse && ActorInUse != CurrentInteractableActor)
	{
		ReleaseActorInUse();
	}
	if (!CurrentInteractableActor) { return; }
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
		EndLocation,
		ECollisionChannel::ECC_Visibility,
		QueryParams
	);

	/** If the line trace hits an object other than the target actor, we assume the target actor is occluded. */
	const bool IsOccluded = IsHit && OcclusionTraceHitResult.GetActor() && OcclusionTraceHitResult.GetActor() != Actor;
	return IsOccluded;
}

UObject* UPlayerInteractionComponent::FindInteractableObject(AActor* Actor) const
{
	if (!Actor) { return nullptr; }
	 UObject* InteractableObject {nullptr};
	
	/** Check if the actor implements the IInteractableObject interface. */
	if (Actor->GetClass()->ImplementsInterface(UInteractableObject::StaticClass()))
	{
		InteractableObject = Actor;
	}
	
	/** If the actor does not implement the IInteractableObject interface, try to find a component that does.*/
	else if (UActorComponent* InteractableComponent {FindInteractableComponent(Actor)})
	{
		InteractableObject = InteractableComponent;
	}
	
	return InteractableObject;
}

UActorComponent* UPlayerInteractionComponent::FindInteractableComponent(const AActor* Actor) const
{
	if (!Actor) { return nullptr; }
	TSet<UActorComponent*> Components {Actor->GetComponents()};
	if (Components.IsEmpty()) { return nullptr; }
	for (UActorComponent* Component : Components)
	{
		if (Component->GetClass()->ImplementsInterface(UInteractableObject::StaticClass()))
		{
			return Component;
		}
	}
	return nullptr;
}

UObject* UPlayerInteractionComponent::BeginInteraction(const EInteractionActionType Type)
{
	if (!CurrentInteractableActor) { return nullptr; }
	UObject* InteractableObject {FindInteractableObject(CurrentInteractableActor)};
	if (!InteractableObject) { return nullptr; }
	
	/** If the player performs a primary interaction action and the interactable object can be 'used',
	*	call the use IInteractableObject interface function on the object.
	*	If the player performs a secondary interaction action and the interactable object can be grabbed,
	*	pass the object to the grab component. */
	switch (Type)
	{
	case EInteractionActionType::Primary:
		{
			const EInteractionType InteractionType {IInteractableObject::Execute_GetInteractionType(InteractableObject)};
			if (InteractionType == EInteractionType::Usable || InteractionType == EInteractionType::Handleable)
			{
				const bool IsUsed {IInteractableObject::Execute_Use(InteractableObject, GetOwner())};
		
				if (IsUsed)
				{
					/** Request the trigger type from the interactable object. If the object is of type press-and-hold,
					 *	store the pointer to the interactable object. */
					const EInteractionTriggerType TriggerType {IInteractableObject::Execute_GetInteractionTriggerType(InteractableObject)};
					if (TriggerType == EInteractionTriggerType::PressAndHold)
					{
						ActorInUse = CurrentInteractableActor;
					}
				}
			}
		}
		break;
		
	case EInteractionActionType::Secondary:
		{
			const EInteractionType InteractionType {IInteractableObject::Execute_GetInteractionType(InteractableObject)};
			if (InteractionType ==EInteractionType::Grabbable ||
				InteractionType == EInteractionType::GrabUsable ||
				InteractionType == EInteractionType::Handleable )
			{
				GrabComponent->GrabObject(CurrentInteractableActor);
				UE_LOG(LogTemp, Error, TEXT("Secondary Action Pressed, Actor is grabbable.")) //TODO: Remove this.
			}
			
		}
	default: break;
	}
	EventBeginInteraction(Type, InteractableObject);
	return InteractableObject;
}

UObject* UPlayerInteractionComponent::EndInteraction(const EInteractionActionType Type)
{
	switch (Type)
	{
	case EInteractionActionType::Primary:
		{
			if (ActorInUse) { ReleaseActorInUse(); }
		}
		break;
		
	case EInteractionActionType::Secondary:
		{
			if (GrabComponent)
			{
				GrabComponent->ReleaseObject();
			}
		}

	default: break;
	}
	return nullptr;
}

void UPlayerInteractionComponent::ReleaseActorInUse()
{
	if (!ActorInUse) { return; }
	UObject* InteractableObject {FindInteractableObject(ActorInUse)};
	if (!InteractableObject)
	{
		ActorInUse = nullptr;
		return;
	}
	bool IsReleased {IInteractableObject::Execute_Disuse(InteractableObject, GetOwner())};
	if (IsReleased) { ActorInUse = nullptr; }
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

