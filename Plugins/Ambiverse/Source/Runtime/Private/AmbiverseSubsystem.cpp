// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#include "AmbiverseSubsystem.h"
#include "..\Core\Public\LogCategories.h"
#include "SoundscapeManager.h"
#include "ParameterManager.h"
#include "SoundSourcePool.h"
#include "DebugComponent.h"

/** Using a macro here instead of a templated function due to to binding errors caused by TStrongObjectPtr overloaded
 *	assignment operator. */
#define INIT_SUBSYSTEM_COMPONENT(ComponentType, ComponentPtr) \
	ComponentPtr.Reset(NewObject<ComponentType>(this)); \
	if (ComponentPtr.IsValid()) { \
		ComponentPtr->Initialize(this); \
		SubsystemComponents.Add(ComponentPtr.Get()); } \

#define DEINIT_SUBSYSTEM_COMPONENT(ComponentPtr) \
	if (ComponentPtr.IsValid()) { \
		ComponentPtr->Deinitialize(this); \
		SubsystemComponents.Remove(ComponentPtr.Get()); \
		ComponentPtr.Reset(); \
		ComponentPtr->ConditionalBeginDestroy(); } \

UAmbiverseSubsystem::UAmbiverseSubsystem()
{
}

void UAmbiverseSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
#if UE_SERVER
	return;
#endif
	
	INIT_SUBSYSTEM_COMPONENT(UParameterManager, ParameterManager);
	INIT_SUBSYSTEM_COMPONENT(USoundSourcePool, SoundSourcePool);
	INIT_SUBSYSTEM_COMPONENT(USoundscapeManager, SoundscapeManager);

#if !UE_BUILD_SHIPPING
	INIT_SUBSYSTEM_COMPONENT(UDebugComponent, DebugComponent);
#endif

	Super::Initialize(Collection);
}

void UAmbiverseSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	
	UE_LOG(LogAmbiverse, Log, TEXT("Ambiverse subsystem initialized successfully."))
}

void UAmbiverseSubsystem::Deinitialize()
{
#if UE_SERVER
	return;
#endif
	
	DEINIT_SUBSYSTEM_COMPONENT(ParameterManager);
	DEINIT_SUBSYSTEM_COMPONENT(SoundSourcePool);
	DEINIT_SUBSYSTEM_COMPONENT(SoundscapeManager);

#if !UE_BUILD_SHIPPING
	DEINIT_SUBSYSTEM_COMPONENT(DebugComponent);
#endif
	
	Super::Deinitialize();
}


