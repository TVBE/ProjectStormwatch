// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#include "AmbiverseSubsystem.h"
#include "AmbiverseDistributorAsset.h"
#include "AmbiverseDistributionManager.h"
#include "AmbiverseElementAsset.h"
#include "AmbiverseElementManager.h"
#include "AmbiverseLayerManager.h"
#include "AmbiverseParameterManager.h"
#include "AmbiverseElementInstance.h"
#include "AmbiverseLayerInstance.h"
#include "AmbiverseSoundSourceData.h"
#include "AmbiverseSoundSourceManager.h"
#include "AmbiverseVisualisationComponent.h"

DEFINE_LOG_CATEGORY_CLASS(UAmbiverseSubsystem, LogAmbiverseSubsystem);

UAmbiverseSubsystem::UAmbiverseSubsystem()
{
}

void UAmbiverseSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	LayerManager = NewObject<UAmbiverseLayerManager>(this);
	if (LayerManager) { LayerManager->Initialize(this); }
	
	ParameterManager = NewObject<UAmbiverseParameterManager>(this);
	if (ParameterManager) { ParameterManager->Initialize(this); }

	SoundSourceManager = NewObject<UAmbiverseSoundSourceManager>(this);
	if (SoundSourceManager) { SoundSourceManager->Initialize(this); }

	DistributorManager = NewObject<UAmbiverseDistributionManager>(this);
	if (DistributorManager) { DistributorManager->Initialize(this); }

	ElementManager = NewObject<UAmbiverseElementManager>(this);
	if (ElementManager) { ElementManager->Initialize(this); }

#if !UE_BUILD_SHIPPING
	VisualisationComponent.Reset(NewObject<UAmbiverseVisualisationComponent>(this));
#endif
}

void UAmbiverseSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	
	UE_LOG(LogAmbiverseSubsystem, Log, TEXT("Adaptive Ambience System initialized successfully."))
}

void UAmbiverseSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (LayerManager && LayerManager->IsInitialized)
	{
		LayerManager->Tick(DeltaTime);
	}
	if (ElementManager && ElementManager->IsInitialized)
	{
		ElementManager->Tick(DeltaTime);
	}
}

bool UAmbiverseSubsystem::AddAmbiverseLayer(UAmbiverseLayerAsset* Asset)
{
	if (!Asset || !LayerManager) { return false; }
	
	return LayerManager->RegisterAmbiverseLayer(Asset);
}

bool UAmbiverseSubsystem::RemoveAmbiverseLayer(UAmbiverseLayerAsset* Asset, const bool ForceStop, const float FadeTime)
{
	if (!Asset) { return false; }

	return LayerManager->UnregisterAmbiverseLayer(Asset, ForceStop, FadeTime);
}

void UAmbiverseSubsystem::Deinitialize()
{
	if (LayerManager)
	{
		LayerManager->Deinitialize(this);
		LayerManager = nullptr;
	}
	if (ParameterManager)
	{
		ParameterManager->Deinitialize(this);
		SoundSourceManager = nullptr;
	}
	if (SoundSourceManager)
	{
		SoundSourceManager->Deinitialize(this);
		SoundSourceManager = nullptr;
	}
	if (DistributorManager)
	{
		DistributorManager->Deinitialize(this);
		DistributorManager = nullptr;
	}
	if (ElementManager)
	{
		ElementManager->Deinitialize(this);
		ElementManager = nullptr;
	}

#if !UE_BUILD_SHIPPING
	if (VisualisationComponent.IsValid())
	{
		VisualisationComponent->ConditionalBeginDestroy();
		VisualisationComponent.Reset();
	}
#endif
	
	Super::Deinitialize();
}


