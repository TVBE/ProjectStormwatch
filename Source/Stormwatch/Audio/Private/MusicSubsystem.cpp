// Copyright (c) 2022-present Barrelhouse. All rights reserved.
// Written by Tim Verberne.

#include "MusicSubsystem.h"
#include "Components/AudioComponent.h"
#include "MetasoundSource.h"
#include "MusicScript.h"

DEFINE_LOG_CATEGORY_CLASS(UMusicSubsystem, LogMusic);

void UMusicSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogMusic, Error, TEXT("Music subsystem could not find world object."))
		return;
	}

	AudioComponent = NewObject<UAudioComponent>(World);
	if (!AudioComponent)
	{
		UE_LOG(LogMusic, Error, TEXT("Music subsystem could not instantiate audio component. "))
		return;
	}
	
	AudioComponent->bAutoDestroy = true;
	AudioComponent->bAllowSpatialization = false;
	AudioComponent->bIsUISound = true;
	AudioComponent->RegisterComponentWithWorld(World);

	IConsoleManager::Get().RegisterConsoleCommand(TEXT("a.SetMusicEnabled"), TEXT("Enable or disable music"), ECVF_Default);

	UE_LOG(LogMusic, Log, TEXT("Successfully initialized music subsystem."));
}

void UMusicSubsystem::Deinitialize()
{
	if (AudioComponent)
	{
		AudioComponent->Stop();
		AudioComponent->DestroyComponent();
		AudioComponent = nullptr;
	}

	MusicScript = nullptr;
	
	Super::Deinitialize();
}

void UMusicSubsystem::ActivateMusicSubystem(UMusicSettings* Settings)
{
	if (!Settings)
	{
		UE_LOG(LogMusic, Error, TEXT("No settings were provided for music subsystem on activation."));
		return;
	}

	UMetaSoundSource* ResolvedMetaSoundSource;
	if (!Settings->MetasoundSourceAsset.IsValid())
	{
		ResolvedMetaSoundSource = Settings->MetasoundSourceAsset.LoadSynchronous();
	}
	else
	{
		ResolvedMetaSoundSource = Settings->MetasoundSourceAsset.Get();
	}
	
	if (ResolvedMetaSoundSource && AudioComponent)
	{
		AudioComponent->SetSound(ResolvedMetaSoundSource);
		AudioComponent->Play();
	}
	else
	{
		UE_LOG(LogMusic, Error, TEXT("Failed to load MetaSoundSource asset from MusicSettings asset."));
		return;
	}
	
	if (const UClass* MusicScriptClass = Settings->MusicScriptClass)
	{
		MusicScript = NewObject<UMusicScript>(this, MusicScriptClass);
		if (MusicScript)
		{
			MusicScript->AudioComponent = AudioComponent;
			MusicScript->Initialize();
		}
	}
	else
	{
		UE_LOG(LogMusic, Error, TEXT("Failed to instantiate MusicScript from MusicSettings asset."));
		return;
	}
	UE_LOG(LogMusic, Log, TEXT("Activated music subsystem."));
}



