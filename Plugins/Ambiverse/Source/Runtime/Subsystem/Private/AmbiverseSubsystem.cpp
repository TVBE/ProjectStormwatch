// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#include "AmbiverseSubsystem.h"
#include "AmbiverseDistributionManager.h"
#include "AmbiverseElementManager.h"
#include "AmbiverseLayerManager.h"
#include "AmbiverseParameterManager.h"
#include "AmbiverseSoundSourceManager.h"
#include "AmbiverseDebugVisualizationComponent.h"

DEFINE_LOG_CATEGORY_CLASS(UAmbiverseSubsystem, LogAmbiverseSubsystem);

/** Using a macro here instead of a templated function due to to binding errors caused by TStrongObjectPtr overloaded
 *	assignment operator, which returns an rvalue. */
#define CREATE_AND_INIT_SUBSYSTEM_COMPONENT(ComponentType, ComponentPtr) \
	ComponentPtr.Reset(NewObject<ComponentType>(this)); \
		if (ComponentPtr.IsValid()) { \
			ComponentPtr->Initialize(this); \
			SubsystemComponents.Add(ComponentPtr.Get()); \
}

UAmbiverseSubsystem::UAmbiverseSubsystem()
{
}

void UAmbiverseSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	CREATE_AND_INIT_SUBSYSTEM_COMPONENT(UAmbiverseLayerManager, LayerManager);
	CREATE_AND_INIT_SUBSYSTEM_COMPONENT(UAmbiverseParameterManager, ParameterManager);
	CREATE_AND_INIT_SUBSYSTEM_COMPONENT(UAmbiverseSoundSourceManager, SoundSourceManager);
	CREATE_AND_INIT_SUBSYSTEM_COMPONENT(UAmbiverseDistributionManager, DistributorManager);
	CREATE_AND_INIT_SUBSYSTEM_COMPONENT(UAmbiverseElementManager, ElementManager);

#if !UE_BUILD_SHIPPING
	CREATE_AND_INIT_SUBSYSTEM_COMPONENT(UAmbiverseDebugVisualizationComponent, Debugger);
#endif

	Super::Initialize(Collection);
}

void UAmbiverseSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	
	UE_LOG(LogAmbiverseSubsystem, Log, TEXT("Adaptive Ambience System initialized successfully."))
}

void UAmbiverseSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (UAmbiverseSubsystemComponent* Component : SubsystemComponents)
	{
		//check (Component)
		if (Component) {continue; }
		Component->Tick(DeltaTime);
		UE_LOG(LogTemp, Error, TEXT("Tick"))
	}
}

bool UAmbiverseSubsystem::AddAmbiverseLayer(UAmbiverseLayerAsset* Asset)
{
	check (LayerManager)
	
	return LayerManager->RegisterAmbiverseLayer(Asset);
}

bool UAmbiverseSubsystem::RemoveAmbiverseLayer(UAmbiverseLayerAsset* Asset, const bool ForceStop, const float FadeTime)
{
	check (LayerManager)

	return LayerManager->UnregisterAmbiverseLayer(Asset, ForceStop, FadeTime);
}

void UAmbiverseSubsystem::Deinitialize()
{
	for (UAmbiverseSubsystemComponent* Component : SubsystemComponents)
	{
		if (Component)
		{
			Component->Deinitialize(this);
			Component->ConditionalBeginDestroy();
		}
	}
	
	Super::Deinitialize();
}


