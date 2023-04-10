// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#include "PlayerUseComponent.h"

#include "UsableObjectInterface"

UPlayerUseComponent::UPlayerUseComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerUseComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPlayerUseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UPlayerUseComponent::BeginUse(UObject* UsableObject)
{
	if (!UsableObject || !UsableObject->Implements<IUsableObject>()) { return false; }

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

bool UPlayerUseComponent::EndUse()
{
	if (!ObjectInUse || !ObjectInUse->Implements<IUsableObject>()) { return false; }
	IUsableObject::Execute_EndUse(ObjectInUse, GetOwner());
	ObjectInUse = nullptr;
	ActorInUse = nullptr;
	return true;
}

