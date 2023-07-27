// Copyright (c) 2022-present Nino Saglia. All Rights Reserved.
// Written by Nino Saglia.

#include "ReacousticSettings.h"
#include "ReacousticSubsystem.h"
#include "Reacoustic.h"
#include "AudioAnalyzerNRT.h"
#include "OnsetNRT.h"
#include "Engine/DataTable.h"
#include "Evaluation/Blending/MovieSceneBlendType.h"
#include "Serialization/JsonTypes.h"

UReacousticProjectSettings::UReacousticProjectSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

#if WITH_EDITOR



void UReacousticProjectSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

#endif