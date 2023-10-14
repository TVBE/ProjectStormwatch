// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHPlayerUseComponent.h"
#include "UsableObjectInterface.h"

UBHPlayerUseComponent::UBHPlayerUseComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UBHPlayerUseComponent::BeginPlay()
{
	Super::BeginPlay();
}

AActor* UBHPlayerUseComponent::GetActorFromObject(UObject* Object) const
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

void UBHPlayerUseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UBHPlayerUseComponent::BeginUse(UObject* UsableObject)
{
	if (!UsableObject || !UsableObject->Implements<UUsableObject>()) { return false; }

	if (ObjectInUse)
	{
		EndUse();
	}
	
	IUsableObject::Execute_BeginUse(UsableObject, GetOwner());
	if (IUsableObject::Execute_GetUseType(UsableObject) == EUseType::PressAndHold)
	{
		ObjectInUse = UsableObject;
		ActorInUse = GetActorFromObject(UsableObject);
	}
	return true;
}

bool UBHPlayerUseComponent::EndUse()
{
	if (!ObjectInUse || !ObjectInUse->Implements<UUsableObject>()) { return false; }
	IUsableObject::Execute_EndUse(ObjectInUse, GetOwner());
	ObjectInUse = nullptr;
	ActorInUse = nullptr;
	return true;
}

