// Copyright (c) 2023-present Tim Verberne
// This source code is part of the Ambiverse plugin

#include "AmbiverseSubsystem.h"

#include "AmbiverseDistributorManager.h"
#include "AmbiverseLayer.h"
#include "AmbiverseLayerManager.h"
#include "AmbiverseParameterManager.h"
#include "AmbiverseSoundSourceData.h"
#include "AmbiverseSoundSourceManager.h"

DEFINE_LOG_CATEGORY_CLASS(UAmbiverseSubsystem, LogAmbiverseSubsystem);

void UAmbiverseSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	LayerManager = NewObject<UAmbiverseLayerManager>(this);
	if (LayerManager) { LayerManager->Initialize(this); }
	
	ParameterManager = NewObject<UAmbiverseParameterManager>(this);
	if (ParameterManager) { ParameterManager->Initialize(this); }

	SoundSourceManager = NewObject<UAmbiverseSoundSourceManager>(this);
	if (SoundSourceManager) { SoundSourceManager->Initialize(this); }

	DistributorManager = NewObject<UAmbiverseDistributorManager>(this);
	if (DistributorManager) { DistributorManager->Initialize(this); }

#if !UE_BUILD_SHIPPING
	SoundSourceVisualisationConsoleCommand = MakeUnique<FAutoConsoleCommand>(
		TEXT("av.EnableSoundSourceVisualisation"),
		TEXT("Enable or disable sound source visualisation."),
		FConsoleCommandWithArgsDelegate::CreateLambda([this](const TArray<FString>& Args)
		{
			if (Args.Num() > 0)
			{
				SetSoundSourceVisualisationEnabled(FCString::Atoi(*Args[0]) != 0);
			}
		})
	);
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

	UpdateActiveLayers(DeltaTime);
}

void UAmbiverseSubsystem::UpdateActiveLayers(const float DeltaTime)
{
	if (LayerManager->GetLayerRegistry().IsEmpty()) { return; }
	
	for (UAmbiverseLayer* Layer : LayerManager->GetLayerRegistry())
	{
		if (Layer->SoundQueue.IsEmpty()) { continue; }
		
		for (FAmbiverseLayerQueueEntry& SoundQueueEntry : Layer->SoundQueue)
		{
			if (SoundQueueEntry.Time != 0)
			{
				const float ScaleFactor {(SoundQueueEntry.Time - DeltaTime) / SoundQueueEntry.Time};
				
				SoundQueueEntry.ReferenceTime *= ScaleFactor;
			}
			
			SoundQueueEntry.Time -= DeltaTime;

			if (SoundQueueEntry.Time <= 0)
			{
				ProcessAmbienceLayerQueue(Layer, SoundQueueEntry);
			}
		}
	}
}

void UAmbiverseSubsystem::ProcessAmbienceLayerQueue(UAmbiverseLayer* Layer, FAmbiverseLayerQueueEntry& Entry)
{
	if (!Layer) { return; }
	
	Entry.ReferenceTime = FMath::RandRange(Entry.SoundData.DelayMin, Entry.SoundData.DelayMax);

	float DensityModifier {1.0f};
	float VolumeModifier {1.0f};

	ParameterManager->GetScalarsForEntry(DensityModifier, VolumeModifier, Layer, Entry);

	Entry.Time = Entry.ReferenceTime * DensityModifier;

	/** We try to get the location of the listener here.*/
	FVector CameraLocation;
	bool IsCameraValid {false};
		
	if (APlayerController* PlayerController {GetWorld()->GetFirstPlayerController()})
	{
		if (const APlayerCameraManager* CameraManager {PlayerController->PlayerCameraManager})
		{
			CameraLocation = CameraManager->GetCameraLocation();
			IsCameraValid = true;
		}
	}

	if (!IsCameraValid) { return; }

	/** Prepare the sound source data. */
	FAmbiverseSoundSourceData SoundSourceData {FAmbiverseSoundSourceData()};
		
	SoundSourceData.Transform = FAmbiverseSoundDistributionData::GetSoundTransform(Entry.SoundData.DistributionData, CameraLocation);
	SoundSourceData.Sound = FAmbiverseProceduralSoundData::GetSoundFromMap(Entry.SoundData.Sounds);
	SoundSourceData.Volume = Entry.SoundData.Volume;
	SoundSourceData.Name = Entry.SoundData.Name;
	SoundSourceData.Layer = Layer;

	DrawDebugSphere(GetWorld(), SoundSourceData.Transform.GetLocation(), 100.0f, 12, FColor::Red, false, 10.0f);
	
	SoundSourceManager->InitiateSoundSource(SoundSourceData);
	UE_LOG(LogAmbiverseSubsystem, Warning, TEXT("Update"))
}

float UAmbiverseSubsystem::GetSoundInterval(const UAmbiverseLayer* Layer, const FAmbiverseLayerQueueEntry& Entry)
{
	if (!Layer) { return -1.0f; }
	float Interval {FMath::RandRange(Entry.SoundData.DelayMin, Entry.SoundData.DelayMax)};
	Interval /= Layer->LayerDensity;
	return Interval;
}

float UAmbiverseSubsystem::GetSoundVolume(const UAmbiverseLayer* Layer, const FAmbiverseLayerQueueEntry& Entry)
{
	if (!Layer) { return -1.0f; }
	float Volume {Entry.SoundData.Volume};
	Volume *= Layer->LayerVolume;
	return Volume;
}

void UAmbiverseSubsystem::HandleParameterChanged()
{
	if (!LayerManager || LayerManager->GetLayerRegistry().IsEmpty()) { return; }

	UE_LOG(LogTemp, Warning, TEXT("Parameter Changed!"))

	TArray<UAmbiverseLayer*> Layers {LayerManager->GetLayerRegistry()};
	for (UAmbiverseLayer* Layer : Layers)
	{
		for (FAmbiverseLayerQueueEntry Entry : Layer->SoundQueue)
		{
			float DensityScalar {1.0f};
			float VolumeScalar {1.0f};
			
			ParameterManager->GetScalarsForEntry(DensityScalar, VolumeScalar, Layer, Entry);

			Entry.Time = Entry.ReferenceTime * DensityScalar;
		}
	}
}

#if !UE_BUILD_SHIPPING
void UAmbiverseSubsystem::SetSoundSourceVisualisationEnabled(bool IsEnabled)
{
	if (SoundSourceManager)
	{
		SoundSourceManager->SetSoundSourceVisualisationEnabled(IsEnabled);
	}
}
#endif

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
	
	Super::Deinitialize();
}


