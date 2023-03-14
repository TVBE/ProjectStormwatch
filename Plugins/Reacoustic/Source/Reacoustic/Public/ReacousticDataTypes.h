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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ReacousticSoundData, Meta = (ClampMin = "-50.0", ClampMax = "50.0", UIMin = "-50.0", UIMax = "50.0"))
	float Gain;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ReacousticSoundData)
	float MaxSpeedScalar;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ReacousticSoundData)
	float ImpulseLength {5.0f};

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
	UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh To Audio Map")
	int32 ReacousticSoundDataRef;
};

USTRUCT(BlueprintType)
struct FPhysicalMaterialToAudioMapEntry
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EPhysicalSurface To Audio Map")
	TEnumAsByte<EPhysicalSurface> SurfaceType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EPhysicalSurface To Audio Map")
	int32 ReacousticSoundDataRef;
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
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="HitSound")
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
	TObjectPtr<USoundAttenuation> Sound_Attenuation {10.0}
	;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Sound_Concurrency", MakeStructureDefaultValue="None"))
	TObjectPtr<USoundConcurrency> Sound_Concurrency;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="ImpulseLength", MakeStructureDefaultValue="0.600000"))
	double ImpulseLength;

	
};

USTRUCT(BlueprintType)
struct FReacousticSurfaces
{
	GENERATED_USTRUCT_BODY()
public:
	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Gain_Db", MakeStructureDefaultValue="0.000000"))
	double Gain_Db;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="HitSound", MakeStructureDefaultValue="None"))
	TObjectPtr<USoundWave> HitSound;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Material", MakeStructureDefaultValue="SurfaceType_Default"))
	TEnumAsByte<EPhysicalSurface> Material;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="SurfaceDampening%", MakeStructureDefaultValue="0.000000"))
	double SurfaceDampeningPercentage;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="SlidingRollingSound", MakeStructureDefaultValue="None"))
	TObjectPtr<USoundWave> SlidingRollingSound;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="MaxSpeedScalar", MakeStructureDefaultValue="1.000000"))
	double MaxSpeedScalar;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="ImpulseLength", MakeStructureDefaultValue="0.500000"))
	double ImpulseLength;
};
