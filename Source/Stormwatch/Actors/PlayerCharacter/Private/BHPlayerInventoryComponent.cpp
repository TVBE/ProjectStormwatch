// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHPlayerInventoryComponent.h"
#include "..\..\..\Interfaces\Public\BHInventoryObjectInterface.h"
#include "BHPlayerInteractionComponent.h"

UBHPlayerInventoryComponent::UBHPlayerInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBHPlayerInventoryComponent::OnComponentCreated()
{
	Super::OnComponentCreated();

	/** Initialize the hotbar.*/
	for (int i = 0; i < HotbarSlots; ++i)
	{
		Hotbar.Add(nullptr);
	}
}

void UBHPlayerInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

UObject* UBHPlayerInventoryComponent::FindInventoryObject(AActor* Actor) const
{
	if (!Actor) { return nullptr; }
	UObject* InventoryObject = nullptr;
	
	/** Check if the actor implements the IInventoryObject interface. */
	if (Actor->GetClass()->ImplementsInterface(UBHInventoryObject::StaticClass()))
	{
		InventoryObject = Actor;
	}
	
	/** If the actor does not implement the IInventoryObject interface, try to find a component that does.*/
	else if (UActorComponent* InventoryComponent = FindInventoryComponent(Actor))
	{
		InventoryObject = InventoryComponent;
	}
	
	return InventoryObject;
}

bool UBHPlayerInventoryComponent::AddActorToInventory(AActor* Actor)
{
	return false;
	// if (!Actor) { return false; }
	//
	// /** Search for the first UObject that implements the IInventoryObject interface.
	//  *	This can be the actor itself, or a component registered to the actor. */
	// if (UObject* InventoryObject = FindInventoryObject(Actor))
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("This Worked!"))
	// 	if (!IBHInventoryObject::Execute_CanAddToInventory(InventoryObject, GetOwner())) {return false; }
	// 	
	// 	/** If the current hotbar slot is already filled, iterate through the array to try and find an empty slot that can fit the actor. */
	// 	if (SelectedActor)
	// 	{
	// 		int32 EmptySlotIndex = -1;
	// 		for (int32 i = 0; i < Hotbar.Num(); ++i)
	// 		{
	// 			if (!Hotbar[i])
	// 			{
	// 				EmptySlotIndex = i;
	// 				Hotbar[i] = Actor;
	// 				break;
	// 			}
	// 		}
	// 		if (EmptySlotIndex >= 0)
	// 		{
	// 			Hotbar[EmptySlotIndex] = Actor;
	// 		}
	// 		else
	// 		{
	// 			return false;
	// 		}
	// 	}
	// 	else
	// 	{
	// 		Hotbar[SelectedSlot] = Actor;
	// 	}
	// 	IBHInventoryObject::Execute_AddToInventory(InventoryObject, GetOwner());
	// 	return true;
	// }
	// return false;
}

AActor* UBHPlayerInventoryComponent::TakeActorFromInventory()
{
	return nullptr;
	// if(SelectedActor)
	// {
	// 	if (UObject* InventoryObject = FindInventoryObject(SelectedActor))
	// 	{
	// 		AActor* TakenActor = SelectedActor;
	// 		Hotbar[SelectedSlot] = nullptr;
	//
	// 		//if (const AActor* Owner = GetOwner())
	// 		//{
	// 		//	const FVector ForwardVector = Owner->GetActorForwardVector();
	// 		//	TakenActor->SetActorLocation(FVector(ForwardVector.X * 80, ForwardVector.Y * 80, 0));
	// 		//}
	// 		
	// 		IBHInventoryObject::Execute_TakeFromInventory(InventoryObject, GetOwner());
	// 		return TakenActor;
	// 	}
	// }
	// return nullptr;
}

UActorComponent* UBHPlayerInventoryComponent::FindInventoryComponent(const AActor* Actor) const
{
	return nullptr;
	// if (!Actor) { return nullptr; }
	// TSet<UActorComponent*> Components {Actor->GetComponents()};
	// if (Components.IsEmpty()) { return nullptr; }
	// for (UActorComponent* Component : Components)
	// {
	// 	if (Component->GetClass()->ImplementsInterface(UBHInventoryObject::StaticClass()))
	// 	{
	// 		return Component;
	// 	}
	// }
	// return nullptr;
}


void UBHPlayerInventoryComponent::HandleInteractableActorChanged(AActor* InteractableActor)
{
	
}



