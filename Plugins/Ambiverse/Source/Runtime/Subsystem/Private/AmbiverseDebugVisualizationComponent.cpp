// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#include "AmbiverseDebugVisualizationComponent.h"
#include "AmbiverseSoundSourceManager.h"
#include "AmbiverseSubsystem.h"

void UAmbiverseDebugVisualizationComponent::Initialize(UAmbiverseSubsystem* Subsystem)
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

void UAmbiverseDebugVisualizationComponent::Tick(const float DeltaTime)
{
	if (!IsDebugEnabled || !Owner) { return; }

	if (UAmbiverseSoundSourceManager* SoundSourceManager {Owner->GetSoundSourceManager()})
	{
		// TArray<AAmbiverseSoundSource*> ActiveSoundSources {SoundSourceManager->GetAct}
	}
}


void UAmbiverseDebugVisualizationComponent::SetSoundSourceVisualisationEnabled(bool IsEnabled)
{
	
}
