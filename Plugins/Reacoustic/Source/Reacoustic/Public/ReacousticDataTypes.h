// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sound/SoundWave.h"
#include "Sound/SoundConcurrency.h"
#include "Sound/SoundAttenuation.h"
#include "ReacousticDataTypes.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FReacousticSoundData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ReacousticSoundData", Meta = (ClampMin = "-50.0", ClampMax = "50.0", UIMin = "-50.0", UIMax = "50.0"))
	float Gain{0.0};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ReacousticSoundData)
	float MaxSpeedScalar{1.0};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ReacousticSoundData)
	float ImpulseLength{1.5};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Analysis)
	TArray<float> OnsetTimingData;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Analysis)
	TMap<float, float> OnsetDataMap;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Analysis)
	TArray<float> OnsetVolumeData;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Sounds)
	USoundWave* ImpactWaveAsset {nullptr};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Sounds)
	USoundWave* SlidingWaveAsset {nullptr};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Surface, Meta = (ClampMin = "0.0", ClampMax = "100.0", UIMin = "0.0", UIMax = "100.0"))
	float SurfaceDampeningPercentage{0.0};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ReacousticSoundData)
	USoundAttenuation* Attenuation{nullptr};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ReacousticSoundData)
	USoundConcurrency* Concurrency{nullptr};
};

UCLASS(BlueprintType)
class UReacousticSoundDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ReacousticSoundData)
	TArray<FReacousticSoundData> AudioData;
	
};

USTRUCT(BlueprintType)
struct FMeshToAudioMapEntry
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh To Audio Map")
	UStaticMesh* Mesh{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh To Audio Map")
	int32 ReacousticSoundDataRef{0};
};

USTRUCT(BlueprintType)
struct FPhysicalMaterialToAudioMapEntry
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EPhysicalSurface To Audio Map")
	TEnumAsByte<EPhysicalSurface> SurfaceType{SurfaceType_Default};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EPhysicalSurface To Audio Map")
	int32 ReacousticSoundDataRef{0};
	
};

// The map with all references to quickly be able to access the right audio data.
UCLASS(BlueprintType)
class UReacousticSoundDataRef_Map : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = MeshToAudioMap)
	TArray<FMeshToAudioMapEntry> MeshMapEntries;
 
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = MaterialToAudioMap)
	TArray<FPhysicalMaterialToAudioMapEntry> PhysicalMaterialMapEntries;
};

USTRUCT(BlueprintType)
struct FReacousticObjects
{
	GENERATED_USTRUCT_BODY()

public:
	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Gain_Db"))
	double Gain_Db {0.0};

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="HitSound"))
	TObjectPtr<USoundWave> HitSound;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Meshes"))
	TArray<UStaticMesh*> Meshes;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="SlidingRollingSound"))
	TObjectPtr<USoundWave> SlidingRollingSound;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="MaxSpeedScalar", MakeStructureDefaultValue="10.000000"))
	double MaxSpeedScalar;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Sound_Attenuation", MakeStructureDefaultValue="/Script/Engine.SoundAttenuation'/Game/ThirdPartyContent/BallisticsVFX/SFX/Attentuations/ImpactsAttenuation.ImpactsAttenuation'"))
	TObjectPtr<USoundAttenuation> Sound_Attenuation;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Sound_Concurrency", MakeStructureDefaultValue="None"))
	TObjectPtr<USoundConcurrency> Sound_Concurrency;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="ImpulseLength", MakeStructureDefaultValue="0.600000"))
	double ImpulseLength;

	FReacousticObjects()
	: Gain_Db(0.0)
	, HitSound(nullptr)
	, SlidingRollingSound(nullptr)
	, MaxSpeedScalar(10.0)
	, Sound_Attenuation(nullptr)
	, Sound_Concurrency(nullptr)
	, ImpulseLength(0.6)
	{
	}
	
};

USTRUCT(BlueprintType)
struct FReacousticSurfaces
{
	GENERATED_USTRUCT_BODY()
public:
	/** The gain for the hitsound */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Gain_Db", MakeStructureDefaultValue="0.000000"))
	double Gain_Db;

	/** The source sound to be used */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="HitSound", MakeStructureDefaultValue="None"))
	TObjectPtr<USoundWave> HitSound;

	/** Surfacematerial for this sound */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Material", MakeStructureDefaultValue="SurfaceType_Default"))
	TEnumAsByte<EPhysicalSurface> Material;

	/** How much a surface sound would dampen the velocity of the object sound */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="SurfaceDampening%", MakeStructureDefaultValue="0.000000"))
	double SurfaceDampeningPercentage;

	/** Looping sound to be used for when the object is sliding */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="SlidingRollingSound", MakeStructureDefaultValue="None"))
	TObjectPtr<USoundWave> SlidingRollingSound;

	/** How fast the object is expected to hit things eg: a cup would be 1. A basketball 5 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="MaxSpeedScalar", MakeStructureDefaultValue="1.000000"))
	double MaxSpeedScalar;

	/** How long the sound should be */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="ImpulseLength", MakeStructureDefaultValue="0.500000"))
	double ImpulseLength;

	FReacousticSurfaces()
	: Gain_Db(0.0)
	, HitSound(nullptr)
	, Material(SurfaceType_Default)
	, SurfaceDampeningPercentage(0.0)
	, SlidingRollingSound(nullptr)
	, MaxSpeedScalar(1.0)
	, ImpulseLength(0.5)
	{
	}
};
