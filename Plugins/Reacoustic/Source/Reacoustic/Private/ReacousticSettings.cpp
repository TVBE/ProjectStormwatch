// Copyright 2023 Nino Saglia & Tim Verberne


#include "ReacousticSettings.h"
#include "ReacousticSubsystem.h"
#include "Reacoustic.h"
#include "Serialization/JsonTypes.h"



UReacousticProjectSettings::UReacousticProjectSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

#if WITH_EDITOR
void UReacousticProjectSettings::GenerateRuntimeData()
{
}

void UReacousticProjectSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

#endif