// Copyright 2023 Nino Saglia & Tim Verberne


#include "ReacousticSettings.h"

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