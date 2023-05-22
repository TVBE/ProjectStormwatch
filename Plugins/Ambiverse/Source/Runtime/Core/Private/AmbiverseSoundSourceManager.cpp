// Copyright (c) 2022-present Tim Verberne
// This source code is part of the Adaptive Ambience System plugin

#include "AmbiverseSoundSourceManager.h"
#include "AmbiverseSoundSource.h"

DEFINE_LOG_CATEGORY_CLASS(UAmbiverseSoundSourceManager, LogAmbiverseSoundSourceManager);

void UAmbiverseSoundSourceManager::Initialize()
{
}

void UAmbiverseSoundSourceManager::InitiateSoundSource(FAmbiverseSoundSourceData& SoundSourceData)
{
	if (!SoundSourceData.Sound) { return; }
	AAmbiverseSoundSource* SoundSourceInstance {nullptr};
	
	if (Pool.Num() == 0)
	{
		SoundSourceInstance = GetWorld()->SpawnActor<AAmbiverseSoundSource>(AAmbiverseSoundSource::StaticClass());
	}
	else
	{
		for (AAmbiverseSoundSource* SoundSource : Pool)
		{
			SoundSourceInstance = SoundSource;
			break;
		}
		
		if (!SoundSourceInstance)
		{
			SoundSourceInstance = GetWorld()->SpawnActor<AAmbiverseSoundSource>(AAmbiverseSoundSource::StaticClass());
		}
	}
	if (SoundSourceInstance)
	{
		SoundSourceInstance->Initialize(this, SoundSourceData);
		ActiveSoundSources.AddUnique(SoundSourceInstance);
	}
}

void UAmbiverseSoundSourceManager::ReleaseToPool(AAmbiverseSoundSource* SoundSource)
{
	if (!SoundSource) { return; }

	ActiveSoundSources.Remove(SoundSource);
	
	Pool.AddUnique(SoundSource);
}

#if !UE_BUILD_SHIPPING
void UAmbiverseSoundSourceManager::SetSoundSourceVisualisationEnabled(const bool IsEnabled)
{
	EnableSoundSourceVisualisation = IsEnabled;
	
	if (ActiveSoundSources.IsEmpty()) { return; }

	for (AAmbiverseSoundSource* ActiveSoundSource : ActiveSoundSources)
	{
		ActiveSoundSource->IsDebugVisualisationEnabled = IsEnabled;
		if (IsEnabled)
		{
			ActiveSoundSource->SetActorTickEnabled(true);
		}
	}
}
#endif

void UAmbiverseSoundSourceManager::Deinitialize()
{
}



