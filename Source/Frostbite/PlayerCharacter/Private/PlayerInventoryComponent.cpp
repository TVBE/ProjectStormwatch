// Copyright 2023 Barrelhouse


#include "PlayerInventoryComponent.h"

#include "InventoryObjectInterface.h"
#include "PlayerInteractionComponent.h"

// Sets default values for this component's properties
UPlayerInventoryComponent::UPlayerInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayerInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	if(const AActor* PlayerCharacter {GetOwner()})
	{
		InteractionComponent = Cast<UPlayerInteractionComponent>(PlayerCharacter->FindComponentByClass(UPlayerInteractionComponent::StaticClass()));
		if (InteractionComponent)
		{
			InteractionComponent->OnInteractableActorFound.AddDynamic(this, &UPlayerInventoryComponent::HandleInteractableActorChanged);
		}
	}
	
}

UObject* UPlayerInventoryComponent::FindInventoryObject(AActor* Actor) const
{
	if (!Actor) { return nullptr; }
	UObject* InventoryObject {nullptr};
	
	/** Check if the actor implements the IInteractableObject interface. */
	if (Actor->GetClass()->ImplementsInterface(UInventoryObject::StaticClass()))
	{
		InventoryObject = Actor;
	}
	
	/** If the actor does not implement the IInteractableObject interface, try to find a component that does.*/
	else if (UActorComponent* InventoryComponent {FindInventoryComponent(Actor)})
	{
		InventoryObject = InventoryComponent;
	}
	
	return InventoryObject;
}

UActorComponent* UPlayerInventoryComponent::FindInventoryComponent(const AActor* Actor) const
{
	if (!Actor) { return nullptr; }
	TSet<UActorComponent*> Components {Actor->GetComponents()};
	if (Components.IsEmpty()) { return nullptr; }
	for (UActorComponent* Component : Components)
	{
		if (Component->GetClass()->ImplementsInterface(UInventoryObject::StaticClass()))
		{
			return Component;
		}
	}
	return nullptr;
}


void UPlayerInventoryComponent::HandleInteractableActorChanged(AActor* InteractableActor)
{
	
}



