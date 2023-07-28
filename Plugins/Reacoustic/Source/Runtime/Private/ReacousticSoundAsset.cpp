// Copyright (c) 2022-present Nino Saglia. All Rights Reserved.

#include "ReacousticSoundAsset.h"

DEFINE_LOG_CATEGORY_CLASS(UReacousticSoundAsset, LogReacousticSound);

bool UReacousticSoundAsset::HasAssetActions() const
{
	return Super::HasAssetActions();
}

bool UReacousticSoundAsset::ShouldEventTriggerAnalysis(FPropertyChangedEvent& PropertyChangedEvent)
{
	return Super::ShouldEventTriggerAnalysis(PropertyChangedEvent);
}

void UReacousticSoundAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	// Call the base class version of this function first.
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
}

void UReacousticSoundAsset::PreEditChange(FProperty* Property)
{
	Super::PreEditChange(Property);
	// your implementation here
}

void UReacousticSoundAsset::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	// your implementation here
}

void UReacousticSoundAsset::Serialize(FStructuredArchiveRecord Record)
{
	Super::Serialize(Record);
	// your implementation here
}

