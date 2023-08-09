// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#include "DebugComponent.h"
#include "SoundSourcePool.h"
#include "AmbiverseSubsystem.h"

void UDebugComponent::Initialize(UAmbiverseSubsystem* Subsystem)
{
	Super::Initialize(Subsystem);

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
}

void UDebugComponent::Tick(const float DeltaTime)
{
	if (!IsDebugEnabled || !Owner) { return; }

	if (USoundSourcePool* SoundSourcePool {Owner->GetSoundSourcePool()})
	{
		// TArray<AAmbiverseSoundSource*> ActiveSoundSources {SoundSourceManager->GetAct}
	}
}


void UDebugComponent::SetSoundSourceVisualisationEnabled(bool IsEnabled)
{
	
}
