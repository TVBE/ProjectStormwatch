// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#include "FrostbiteFunctionLibrary.h"

#include "DraggableObjectInterface.h"
#include "FrostbiteWorldSubystem.h"
#include "GrabbableObjectInterface.h"
#include "InteractableObjectInterface.h"
#include "InventoryObjectInterface.h"
#include "NightstalkerDirector.h"
#include "PowerConsumerInterface.h"
#include "UsableObjectInterface.h"
#include "SensoryEventManager.h"

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

void UFrostbiteFunctionLibrary::CallFunctionOnActorUsingActorFunctionCaller(FActorFunctionCaller FunctionCaller)
{
	FunctionCaller.CallFunction();
}

APlayerCharacter* UFrostbiteFunctionLibrary::GetFrostbitePlayerCharacter(const UObject* WorldContextObject)
{
	if (!WorldContextObject) { return nullptr; }
	if (const UWorld* World {WorldContextObject->GetWorld()})
	{
		if (const UFrostbiteWorldSubsystem* Subsystem {World->GetSubsystem<UFrostbiteWorldSubsystem>()})
		{
			return Subsystem->GetPlayerCharacter();
		}
	}
	return nullptr;
}

APlayerCharacterController* UFrostbiteFunctionLibrary::GetFrostbitePlayerCharacterController(const UObject* WorldContextObject)
{
	if (!WorldContextObject) { return nullptr; }
	if (const UWorld* World {WorldContextObject->GetWorld()})
	{
		if (const UFrostbiteWorldSubsystem* Subsystem {World->GetSubsystem<UFrostbiteWorldSubsystem>()})
		{
			return Subsystem->GetPlayerController();
		}
	}
	return nullptr;
}

ANightstalker* UFrostbiteFunctionLibrary::GetFrostbiteNightstalker(const UObject* WorldContextObject)
{
	if (!WorldContextObject) { return nullptr; }
	if (const UWorld* World {WorldContextObject->GetWorld()})
	{
		if (const UNightstalkerDirector* Subsystem {World->GetSubsystem<UNightstalkerDirector>()})
		{
			return Subsystem->GetNightstalker();
		}
	}
	return nullptr;
}

void UFrostbiteFunctionLibrary::PlayAuditoryEventAtLocation(const UObject* WorldContextObject,
                                                            const FAuditoryEvent& AuditoryEvent, const FVector& Location)
{
	if (!WorldContextObject)
	{
		return;
	}

	const UWorld* World {WorldContextObject->GetWorld()};
	if (!World)
	{
		return;
	}

	const UNightstalkerDirector* NightstalkerDirector {World->GetSubsystem<UNightstalkerDirector>()};
	if (!NightstalkerDirector)
	{
		return;
	}

	USensoryEventManager* SensoryEventManager {NightstalkerDirector->GetSensoryEventManager()};
	if (!SensoryEventManager)
	{
		return;
	}

	SensoryEventManager->AddAuditoryEventAtLocation(AuditoryEvent, Location);
}

