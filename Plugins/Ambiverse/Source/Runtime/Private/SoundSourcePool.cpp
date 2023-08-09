// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#include "SoundSourcePool.h"
#include "SoundscapeManager.h"
#include "AmbiverseSoundSource.h"
#include "AmbiverseSubsystem.h"

void USoundSourcePool::InitiateSoundSource(FSoundSourceData& SoundSourceData, UAmbiverseElement* ElementInstance)
{
	if (!SoundSourceData.Sound)
	{
		UE_LOG(LogAmbiverseSoundSourceManager, Warning, TEXT("InitiateSoundSource: SoundSourceData contains no valid sound."))
		return;
	}
	
	AAmbiverseSoundSource* SoundSourceInstance {GetSoundSourceFromPool(ElementInstance->RuntimeData.ElementAsset->GetSoundSourceClass())};
	
	if (SoundSourceInstance)
	{
		if(SoundSourceInstance->Initialize(SoundSourceData, ElementInstance))
		{
			SoundSourceInstance->OnFinishedPlayback.BindUObject(this, &USoundSourcePool::HandleSoundSourceFinishedPlayback);
			ActiveSoundSources.AddUnique(SoundSourceInstance);
		}
	}
}

void USoundSourcePool::HandleSoundSourceFinishedPlayback(AAmbiverseSoundSource* SoundSource)
{
	if (!SoundSource) { return; }

	/** We retrieve the associated element from the SoundSource before deinitializing the SoundSource and adding it back to the available pool.
	 *	We pass it to the ElementManager so that it can reschedule the element if it is set to only be rescheduled after finishing playback. */
	if (UAmbiverseElement* ElementInstance {SoundSource->GetAssociatedElement()})
	{
		/** We set the element's LastPlaylocation here. */ //TODO: Move this to a more suitable location.
		ElementInstance->LastPlaylocation = SoundSource->GetActorLocation();
		if (USoundscapeManager* ElementManager {Owner->GetSoundscapeManager()})
		{
			ElementManager->EvaluateFinishedElement(ElementInstance);
		}
	}
	
	SoundSource->Deinitialize();
	
	ActiveSoundSources.Remove(SoundSource);
	Pool.AddUnique(SoundSource);
}

AAmbiverseSoundSource* USoundSourcePool::GetSoundSourceFromPool(TSubclassOf<AAmbiverseSoundSource> Class)
{
	for (int32 i {0}; i < Pool.Num(); i++)
	{
		AAmbiverseSoundSource* SoundSource {Pool[i]};
		if (SoundSource && SoundSource->GetClass() == Class)
		{
			Pool.RemoveAt(i);
			return SoundSource;
		}
	}

	/** If we cannot find a SoundSource of the correct class, we create a new instance and hand it over immediatly. */
	if (UWorld* World {GetWorld()})
	{
		AAmbiverseSoundSource* SoundSource {World->SpawnActor<AAmbiverseSoundSource>(Class)};
		return SoundSource;
	}
	return nullptr;
}

#if !UE_BUILD_SHIPPING
void USoundSourcePool::SetSoundSourceVisualisationEnabled(const bool IsEnabled)
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



