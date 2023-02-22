// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sound/SoundWave.h"
#include "Sound/SoundConcurrency.h"
#include "Sound/SoundAttenuation.h"

#include "ReacousticSoundData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FReacousticSoundData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ReacousticSoundData, Meta = (ClampMin = "-50.0", ClampMax = "50.0", UIMin = "-50.0", UIMax = "50.0"))
	float Gain;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ReacousticSoundData)
	float MaxSpeedScalar;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ReacousticSoundData)
	float ImpulseLength;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Analysis)
	TArray<float> OnsetTimingData;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Analysis)
	TMap<float, float> OnsetDataMap;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Analysis)
	TArray<float> OnsetVolumeData;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Sounds)
	USoundWave* ImpactWaveAsset;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Sounds)
	USoundWave* SlidingWaveAsset;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Surface, Meta = (ClampMin = "0.0", ClampMax = "100.0", UIMin = "0.0", UIMax = "100.0"))
	float SurfaceDampeningPercentage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ReacousticSoundData)
	USoundAttenuation* Attenuation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ReacousticSoundData)
	USoundConcurrency* Concurrency;

	// Constructor with default values.
	FReacousticSoundData()
	{
		Gain = 1.0;
		MaxSpeedScalar = 1.0;
		ImpulseLength = 1.0;
		SurfaceDampeningPercentage = 25.0;
	}
};
