// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#include "RoomAudioComponent.h"
#include "SubmixEffects/SubmixEffectConvolutionReverb.h"

URoomAudioComponent::URoomAudioComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URoomAudioComponent::BeginPlay()
{
	Super::BeginPlay();
}

void URoomAudioComponent::ConstructComponent()
{
	Super::ConstructComponent();

	// ReverbConvolutionReverbPreset = FindMatchingImpulseResponseFromDataTable(ReverbTypes, );
}

USubmixEffectConvolutionReverbPreset* URoomAudioComponent::FindMatchingImpulseResponseFromDataTable(UDataTable* DataTable, const float Volume, const float ShapeRatio, const float Reflectivity)
{
	if (!DataTable) { return nullptr; }

	TArray<FName> RowNames {DataTable->GetRowNames()};
	int Index {-1};
	float MinDifference {FLT_MAX};
	
	for (int32 i {0}; i < RowNames.Num(); i++)
	{
		const FName CurrentRowName {RowNames[i]};

		if (const FRoomReverbSettings* CurrentRow {DataTable->FindRow<FRoomReverbSettings>(CurrentRowName, FString(""))})
		{
			/** weights for each parameter. */
			constexpr float VolumeWeight = 3.0f;
			constexpr float RatioWeight = 1.0f;
			constexpr float ReflectivityWeight = 1.0f;

			/** Value for volume normalization. */
			constexpr float MaxVolume = 500.0f;
			
			const float VolumeDifference {FMath::Abs(FMath::Clamp(Volume, 0, MaxVolume) - CurrentRow->RoomVolume) / MaxVolume};
			const float RatioDifference {FMath::Abs(ShapeRatio - CurrentRow->ShapeRatio)};
			const float ReflectivityDifference {FMath::Abs(Reflectivity - CurrentRow->Reflectivity)};

			const float TotalDifference {VolumeDifference * VolumeWeight + RatioDifference * RatioWeight + ReflectivityDifference * ReflectivityWeight};

			if (TotalDifference < MinDifference)
			{
				MinDifference = TotalDifference;
				Index = i;
			}
		}
	}

	if (Index >= 0 && Index < RowNames.Num())
	{
		const FName MatchedRowName {RowNames[Index]};
		if (const FRoomReverbSettings* MatchedRow {DataTable->FindRow<FRoomReverbSettings>(MatchedRowName, FString(""))})
		{
			return MatchedRow->ImpulseResponse;
		}
	}
	return nullptr;
}

