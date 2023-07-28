// Copyright (c) 2022-present Nino Saglia. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnsetNRT.h"
#include "Sound/SoundWave.h"
#include "Sound/SoundConcurrency.h"
#include "Sound/SoundAttenuation.h"
#include "ReacousticSoundAsset.generated.h"

/** An audio asset that utilizes Onset NRT analysis to select and playback specific transients in real-time. */
UCLASS(Blueprintable, BlueprintType, ClassGroup = "Reacoustic", Meta = (DisplayName = "Reacoustic Sound",
	ShortToolTip = "An auto analyzed sound clip for impact sound playback."))

class REACOUSTIC_API UReacousticSoundAsset : public UOnsetNRT

{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogReacousticSound, Log, All)
	
	/** OnsetNRT */
	bool HasAssetActions() const;
	bool ShouldEventTriggerAnalysis(FPropertyChangedEvent& PropertyChangedEvent);
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent);
	void PreEditChange(FProperty* Property);
	void Serialize(FArchive& Ar);
	void Serialize(FStructuredArchiveRecord Record);

public:


/** BASIC CATEGORY */
	

/** The sound to play when the object is sliding or rolling */
UPROPERTY(EditAnywhere,BlueprintReadOnly, meta=(DisplayName="SlidingRollingSound"), Category = "Basic")
TObjectPtr<USoundWave> SlidingRollingSound;

/** The attenuation settings for the impact sound */
UPROPERTY(EditAnywhere,BlueprintReadOnly, meta=(DisplayName="Sound_Attenuation", MakeStructureDefaultValue="/Script/Engine.SoundAttenuation'/Game/ThirdPartyContent/BallisticsVFX/SFX/Attentuations/ImpactsAttenuation.ImpactsAttenuation'"), Category = "Basic setting")
TObjectPtr<USoundAttenuation> Sound_Attenuation;

/** The sound concurrency settings for the impact sound */
UPROPERTY(EditAnywhere,BlueprintReadOnly, meta=(DisplayName="Sound_Concurrency", MakeStructureDefaultValue="None"), Category = "Basic setting")
TObjectPtr<USoundConcurrency> Sound_Concurrency{nullptr};


/** DYNAMIC BEHAVIOUR CATEGORY */

/** The gain in decibels of the impact sound */
UPROPERTY(EditAnywhere,BlueprintReadOnly, meta=(DisplayName="Gain_Db"), Category = "Dynamic Behaviour")
double Gain_Db {0.0};


/** PHYSICS CATEGORY */

/** How fast the object is expected to hit things eg: a cup would be 1. A basketball 5 */
UPROPERTY(EditAnywhere,BlueprintReadOnly, meta=(DisplayName="MaxSpeedScalar", MakeStructureDefaultValue="1.000000"), Category = "Physics")
double MaxSpeedScalar{1.0};


/** TEMPORAL BEHAVIOUR CATEGORY */

/** The length of the impulse for the impact effect */
UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName="ImpulseLength", MakeStructureDefaultValue="0.600000"), Category = "Temporal behaviour")
double ImpulseLength{0.6};


/** SPECTRAL BEHAVIOUR CATEGORY */

/** The pitch shift in semitones for the impact sound */
UPROPERTY(EditAnywhere,BlueprintReadOnly, meta=(DisplayName="Semitone pitch shift", MakeStructureDefaultValue="0.0"), Category = "Spectral behaviour")
float PitchShift{0.0};

/** Whether to use weight-dependent pitch for the impact sound */
UPROPERTY(EditAnywhere,BlueprintReadOnly, meta=(DisplayName="Use weight-dependent pitch", MakeStructureDefaultValue="false"), Category = "Spectral behaviour")
bool UseWeightDependentPitch{false};

/** The default weight for weight-dependent pitch for the impact sound */
UPROPERTY(EditAnywhere,BlueprintReadOnly, meta=(DisplayName="Default weight", MakeStructureDefaultValue="0.0", EditCondition="UseWeightDependentPitch"), Category = "Spectral behaviour")
float DefaultWeight{10.0};

/** The pitch factor for weight-dependent pitch for the impact sound */
UPROPERTY(EditAnywhere,BlueprintReadOnly, meta=(DisplayName="Weight pitch factor", MakeStructureDefaultValue="0.0", EditCondition="UseWeightDependentPitch"), Category = "Spectral behaviour")
float WeightPitchFactor{0.0};

};
