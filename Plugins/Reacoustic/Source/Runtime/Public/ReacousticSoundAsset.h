// Copyright (c) 2022-present Nino Saglia. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Sound/SoundWave.h"
#include "Sound/SoundConcurrency.h"
#include "Sound/SoundAttenuation.h"
#include "ReacousticSoundAsset.generated.h"

/** An ambiverse element is a single procedural sound. It can be played directly, or used in a layer to create a procedural soundscape. */
UCLASS(Blueprintable, BlueprintType, ClassGroup = "Reacoustic", Meta = (DisplayName = "Reacoustic Sound",
	ShortToolTip = "A single sound element that can be used inside an Ambiverse Layer"))
class REACOUSTIC_API UReacousticSoundAsset : public UObject
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogReacousticSound, Log, All)

private:

	
	/** The gain in decibels of the impact sound */
UPROPERTY(EditAnywhere, meta=(DisplayName="Gain_Db"), Category = "Dynamic Behaviour")
double Gain_Db {0.0};

/** The sound to play when the impact occurs */
UPROPERTY(EditAnywhere, meta=(DisplayName="Sound"), Category = "Basic")
TObjectPtr<USoundWave> HitSound;

/** The sound to play when the object is sliding or rolling */
UPROPERTY(EditAnywhere, meta=(DisplayName="SlidingRollingSound"), Category = "Basic")
TObjectPtr<USoundWave> SlidingRollingSound;

	/** How fast the object is expected to hit things eg: a cup would be 1. A basketball 5 */
UPROPERTY(EditAnywhere, meta=(DisplayName="MaxSpeedScalar", MakeStructureDefaultValue="1.000000"), Category = "Physics")
double MaxSpeedScalar{1.0};

/** The attenuation settings for the impact sound */
UPROPERTY(EditAnywhere, meta=(DisplayName="Sound_Attenuation", MakeStructureDefaultValue="/Script/Engine.SoundAttenuation'/Game/ThirdPartyContent/BallisticsVFX/SFX/Attentuations/ImpactsAttenuation.ImpactsAttenuation'"), Category = "Basic setting")
TObjectPtr<USoundAttenuation> Sound_Attenuation;

/** The sound concurrency settings for the impact sound */
UPROPERTY(EditAnywhere, meta=(DisplayName="Sound_Concurrency", MakeStructureDefaultValue="None"), Category = "Basic setting")
TObjectPtr<USoundConcurrency> Sound_Concurrency{nullptr};

/** The length of the impulse for the impact effect */
UPROPERTY(EditAnywhere, meta=(DisplayName="ImpulseLength", MakeStructureDefaultValue="0.600000"), Category = "Temporal behaviour")
double ImpulseLength{0.6};

/** The pitch shift in semitones for the impact sound */
UPROPERTY(EditAnywhere, meta=(DisplayName="Semitone pitch shift", MakeStructureDefaultValue="0.0"), Category = "Spectral behaviour")
float PitchShift{0.0};

/** Whether to use weight-dependent pitch for the impact sound */
UPROPERTY(EditAnywhere, meta=(DisplayName="Use weight-dependent pitch", MakeStructureDefaultValue="false"), Category = "Spectral behaviour")
bool UseWeightDependentPitch{false};

/** The default weight for weight-dependent pitch for the impact sound */
UPROPERTY(EditAnywhere, meta=(DisplayName="Default weight", MakeStructureDefaultValue="0.0", EditCondition="UseWeightDependentPitch"), Category = "Spectral behaviour")
float DefaultWeight{10.0};

/** The pitch factor for weight-dependent pitch for the impact sound */
UPROPERTY(EditAnywhere, meta=(DisplayName="Weight pitch factor", MakeStructureDefaultValue="0.0", EditCondition="UseWeightDependentPitch"), Category = "Spectral behaviour")
float WeightPitchFactor{0.0};


public:
	bool IsValid {true};
/**
	FORCEINLINE USoundBase* GetSound()
	{
		if (!Sound.IsValid()) { Sound.LoadSynchronous(); }
		return Sound.Get();
	}
	FORCEINLINE TSubclassOf<UAmbiverseDistributorAsset> GetDistributorClass() const { return DistributorClass; }
	FORCEINLINE FAmbiverseSoundDistributionData GetDistributionData() const { return DistributionData; }
	FORCEINLINE TSubclassOf<AAmbiverseSoundSource> GetSoundSourceClass() const { return SoundSourceClass; }
	*/
};
