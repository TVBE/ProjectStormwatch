// Copyright (c) 2022-present Tim Verberne
// This source code is part of the Adaptive Ambience System plugin

#include "AmbiverseSubsystem.h"
#include "AmbiverseLayer.h"
#include "AmbiverseLayerManager.h"
#include "AmbiverseParameterManager.h"
#include "AmbiverseSoundSourceData.h"
#include "AmbiverseSoundSourceManager.h"

DEFINE_LOG_CATEGORY_CLASS(UAmbiverseSubsystem, LogAmbiverseSubsystem);

void UAmbiverseSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	LayerManager = NewObject<UAmbiverseLayerManager>(this);

	SoundSourceManager = NewObject<UAmbiverseSoundSourceManager>(this);
	if (SoundSourceManager)
	{
		SoundSourceManager->Initialize();
	}

	ParameterManager = NewObject<UAmbiverseParameterManager>(this);

	if (ParameterManager)
	{
		ParameterManager->Initialize();
	}


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

	UE_LOG(LogAmbiverseSubsystem, Log, TEXT("Adaptive Ambience System initialized successfully."))
}

void UAmbiverseSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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

			float DensityModifier {1.0f};
			float VolumeModifier {1.0f};

			ParameterManager->GetScalarsForEntry(DensityModifier, VolumeModifier, Layer, SoundQueueEntry);

			SoundQueueEntry.Time = SoundQueueEntry.ReferenceTime * DensityModifier;
		}
	}
}

void UAmbiverseSubsystem::ProcessAmbienceLayerQueue(UAmbiverseLayer* Layer, FAmbiverseLayerQueueEntry& Entry)
{
	if (!Layer) { return; }
	
	Entry.ReferenceTime = FMath::RandRange(Entry.SoundData.DelayMin, Entry.SoundData.DelayMax);

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
	if (SoundSourceManager)
	{
		SoundSourceManager->Deinitialize();
		SoundSourceManager = nullptr;
	}
	if (ParameterManager)
	{
		ParameterManager->Deinitialize();
		SoundSourceManager = nullptr;
	}
	Super::Deinitialize();
}


