// Copyright (c) 2022-present Nino Saglia. All Rights Reserved.

#include "ReacousticSoundAsset.h"

#include "PhysicsProxy/SingleParticlePhysicsProxy.h"

DEFINE_LOG_CATEGORY_CLASS(UReacousticSoundAsset, LogReacousticSound);


bool UReacousticSoundAsset::HasAssetActions() const
{
	return Super::HasAssetActions();
}

bool UReacousticSoundAsset::ShouldEventTriggerAnalysis(FPropertyChangedEvent& PropertyChangedEvent)
{
	return Super::ShouldEventTriggerAnalysis(PropertyChangedEvent);
}

void UReacousticSoundAsset::TriggerOnsetNRTAnalysis()
{
	AnalyzeAudio();
}

void UReacousticSoundAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UReacousticSoundAsset::PreEditChange(FProperty* Property)
{
	Super::PreEditChange(Property);
}

void UReacousticSoundAsset::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
}

void UReacousticSoundAsset::Serialize(FStructuredArchiveRecord Record)
{
	Super::Serialize(Record);
}


TArray<float> UReacousticSoundAsset::GetTimestampHistory() const
{
	return TimestampHistory;
}

void UReacousticSoundAsset::UpdateTimestampHistory(const float& NewElement)
{

	if (TimestampHistory.Num() >= 20)
	{
		TimestampHistory.RemoveAt(0);
	}

	TimestampHistory.Add(NewElement);
}
