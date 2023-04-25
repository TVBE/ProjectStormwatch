// Copyright Notice


#include "FrostbiteFunctionLibrary.h"

#include "DoorInterface.h"
#include "DraggableObjectInterface.h"
#include "GrabbableObjectInterface.h"
#include "InteractableObjectInterface.h"
#include "InventoryObjectInterface.h"
#include "PowerConsumerInterface.h"
#include "UsableObjectInterface.h"

template <typename TInterface>
UActorComponent* FindInteractableComponent(const AActor* Actor)
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

template <typename TInterface>
UObject* FindInteractableObject(AActor* Actor)
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

UObject* UFrostbiteFunctionLibrary::SearchActorForObjectThatImplementsInterface(EFunctionResult& Result,
	AActor* Actor, EFrostbiteInterfaceType Interface)
{
	if (!Actor) { return nullptr; }

	UObject* InterfaceObject {nullptr};

	switch(Interface)
	{
	case EFrostbiteInterfaceType::InteractableObject: InterfaceObject = FindInteractableObject<UInteractableObject>(Actor);
		break;
	case EFrostbiteInterfaceType::UsableObject: InterfaceObject = FindInteractableObject<UUsableObject>(Actor);
		break;
	case EFrostbiteInterfaceType::GrabbableObject: InterfaceObject = FindInteractableObject<UGrabbableObject>(Actor);
		break;
	case EFrostbiteInterfaceType::DraggableObject: InterfaceObject = FindInteractableObject<UDraggableObject>(Actor);
		break;
	case EFrostbiteInterfaceType::InventoryObject: InterfaceObject = FindInteractableObject<UInventoryObject>(Actor);
		break;
	case EFrostbiteInterfaceType::PowerConsumer: InterfaceObject = FindInteractableObject<UPowerConsumer>(Actor);
		break;
	case EFrostbiteInterfaceType::Door: InterfaceObject = FindInteractableObject<UDoor>(Actor);
		break;
	default: break;
	}

	if (InterfaceObject)
	{
		Result = EFunctionResult::Successful;
	}
	else
	{
		Result = EFunctionResult::Unsuccessful;
	}

	return InterfaceObject;
}

