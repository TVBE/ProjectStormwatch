// Copyright (c) 2022-present Tim Verberne
// This source code is part of the Adaptive Ambience System plugin

#include "AmbiverseSubsystem.h"
#include "AmbiverseLayer.h"
#include "AmbiverseSoundSourceData.h"
#include "AmbiverseSoundSourceManager.h"

DEFINE_LOG_CATEGORY_CLASS(UAmbiverseSubsystem, LogAmbiverseSubsystem);

void UAmbiverseSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	SoundSourceManager = NewObject<UAmbiverseSoundSourceManager>(this);

	if(SoundSourceManager)
	{
		SoundSourceManager->Initialize();
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

void UAmbiverseSubsystem::AddAmbienceLayer(UAmbiverseLayer* Layer)
{
	if (!Layer)
	{
		UE_LOG(LogAmbiverseSubsystem, Warning, TEXT("AddAmbienceLayer: No Layer provided."));
		return;
	}

	if (Layer->ProceduralSounds.IsEmpty())
	{
		UE_LOG(LogAmbiverseSubsystem, Warning, TEXT("AddAmbienceLayer: Layer has no procedural sounds: '%s'."), *Layer->GetName());
		return;
	}

	for (FAmbiverseProceduralSoundData& SoundData : Layer->ProceduralSounds)
	{
		FAmbiverseProceduralSoundData::Validate(SoundData);
	}

	bool HasValidSoundData = false;
	for (const FAmbiverseProceduralSoundData& SoundData : Layer->ProceduralSounds)
	{
		if (SoundData.IsValid)
		{
			HasValidSoundData = true;
			break;
		}
	}

	if (!HasValidSoundData)
	{
		UE_LOG(LogAmbiverseSubsystem, Warning, TEXT("AddAmbienceLayer: Layer has no valid procedural sounds: '%s'."), *Layer->GetName());
		return;
	}

	
	if (!FindActiveAmbienceLayer(Layer))
	{
		ActiveLayers.Add(Layer);
		UE_LOG(LogAmbiverseSubsystem, Verbose, TEXT("AddAmbienceLayer: Layer added successfully: '%s'."), *Layer->GetName());
	}
}

void UAmbiverseSubsystem::PopAmbienceLayer(UAmbiverseLayer* Layer)
{
	if (!Layer) { return; }
}

UAmbiverseLayer* UAmbiverseSubsystem::FindActiveAmbienceLayer(const UAmbiverseLayer* LayerToFind) const
{
	for (UAmbiverseLayer* Layer : ActiveLayers)
	{
		if (Layer == LayerToFind)
		{
			return Layer;
		}
	}

	return nullptr;
}

void UAmbiverseSubsystem::ProcessAmbienceLayerQueue(UAmbiverseLayer* Layer, FAmbiverseLayerQueueEntry& Entry)
{
	if (!Layer) { return; }

	Layer->SubtractTimeFromQueue(Entry.Time);
	Entry.Time = FMath::RandRange(Entry.SoundData.DelayMin, Entry.SoundData.DelayMax);

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

	/** Update the timer handle and timer delegate for the layer. */
	if (FAmbiverseLayerQueueEntry LowestTimeEntry; Layer->GetEntryWithLowestTime(LowestTimeEntry))
	{
		const float NewTime {LowestTimeEntry.Time};

		Layer->TimerDelegate.BindUFunction(this, FName("ProcessAmbienceLayerQueue"), Layer, LowestTimeEntry);

		GetWorld()->GetTimerManager().SetTimer(Layer->TimerHandle, Layer->TimerDelegate, NewTime, false);
	}
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
	}
	Super::Deinitialize();
}


