// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHStormwatchGameplayLibrary.h"

#include "..\..\Interfaces\Public\BHDraggableObjectInterface.h"
#include "..\..\Core\Public\BHStormwatchWorldSubystem.h"
#include "..\..\Interfaces\Public\BHGrabbableObjectInterface.h"
#include "..\..\Interfaces\Public\BHInteractableObjectInterface.h"
#include "..\..\Interfaces\Public\BHInventoryObjectInterface.h"
#include "BHNightstalkerDirector.h"
#include "..\..\Interfaces\Public\BHPowerConsumerInterface.h"
#include "..\..\Interfaces\Public\BHUsableObjectInterface.h"
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

UObject* UBHStormwatchGameplayLibrary::SearchActorForObjectThatImplementsInterface(EFunctionResult& Result,
	AActor* Actor, EStormwatchInterfaceType Interface)
{
	if (!Actor) { return nullptr; }

	UObject* InterfaceObject = nullptr;

	switch(Interface)
	{
	case EStormwatchInterfaceType::InteractableObject: InterfaceObject = FindInteractableObject<UBHInteractableObject>(Actor);
		break;
	case EStormwatchInterfaceType::UsableObject: InterfaceObject = FindInteractableObject<UBHUsableObject>(Actor);
		break;
	case EStormwatchInterfaceType::GrabbableObject: InterfaceObject = FindInteractableObject<UBHGrabbableObject>(Actor);
		break;
	case EStormwatchInterfaceType::DraggableObject: InterfaceObject = FindInteractableObject<UBHDraggableObject>(Actor);
		break;
	case EStormwatchInterfaceType::InventoryObject: InterfaceObject = FindInteractableObject<UBHInventoryObject>(Actor);
		break;
	case EStormwatchInterfaceType::PowerConsumer: InterfaceObject = FindInteractableObject<UBHPowerConsumer>(Actor);
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

void UBHStormwatchGameplayLibrary::CallFunctionOnActorUsingActorFunctionCaller(FActorFunctionCaller FunctionCaller)
{
	FunctionCaller.CallFunction();
}

ABHPlayerCharacter* UBHStormwatchGameplayLibrary::GetStormwatchPlayerCharacter(const UObject* WorldContextObject)
{
	if (!WorldContextObject) { return nullptr; }
	if (const UWorld* World = WorldContextObject->GetWorld())
	{
		if (const UBHStormwatchWorldSubsystem* Subsystem = World->GetSubsystem<UBHStormwatchWorldSubsystem>())
		{
			return Subsystem->GetPlayerCharacter();
		}
	}
	return nullptr;
}

ABHPlayerCharacterController* UBHStormwatchGameplayLibrary::GetStormwatchPlayerCharacterController(const UObject* WorldContextObject)
{
	if (!WorldContextObject) { return nullptr; }
	if (const UWorld* World = WorldContextObject->GetWorld())
	{
		if (const UBHStormwatchWorldSubsystem* Subsystem = World->GetSubsystem<UBHStormwatchWorldSubsystem>())
		{
			return Subsystem->GetPlayerController();
		}
	}
	return nullptr;
}

ABHNightstalker* UBHStormwatchGameplayLibrary::GetStormwatchNightstalker(const UObject* WorldContextObject)
{
	if (!WorldContextObject) { return nullptr; }
	if (const UWorld* World = WorldContextObject->GetWorld())
	{
		if (const UBHNightstalkerDirector* Subsystem = World->GetSubsystem<UBHNightstalkerDirector>())
		{
			return Subsystem->GetNightstalker();
		}
	}
	return nullptr;
}

void UBHStormwatchGameplayLibrary::PlayAuditoryEventAtLocation(const UObject* WorldContextObject,
                                                            const FAuditoryEvent& AuditoryEvent, const FVector& Location)
{
	if (!WorldContextObject)
	{
		return;
	}

	const UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		return;
	}

	const UBHNightstalkerDirector* NightstalkerDirector = World->GetSubsystem<UBHNightstalkerDirector>();
	if (!NightstalkerDirector)
	{
		return;
	}

	USensoryEventManager* SensoryEventManager = NightstalkerDirector->GetSensoryEventManager();
	if (!SensoryEventManager)
	{
		return;
	}

	SensoryEventManager->AddAuditoryEventAtLocation(AuditoryEvent, Location);
}

