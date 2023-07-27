// Copyright (c) 2022-present Nino Saglia. All Rights Reserved.
// Written by Nino Saglia.

#pragma once

#include "CoreMinimal.h"
#include "ReacousticSoundAsset.h"
#include "Sound/SoundWave.h"
#include "Sound/SoundConcurrency.h"
#include "Sound/SoundAttenuation.h"
#include "ReacousticDataTypes.generated.h"

USTRUCT(BlueprintType)
struct FMeshToAudioMapEntry
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh To Audio Map")
	UStaticMesh* Mesh{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh To Audio Map")
	UReacousticSoundAsset* ReacousticSound{nullptr};
};

USTRUCT(BlueprintType)
struct FPhysicalMaterialToAudioMapEntry
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EPhysicalSurface To Audio Map")
	TEnumAsByte<EPhysicalSurface> SurfaceType{SurfaceType_Default};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EPhysicalSurface To Audio Map")
	UReacousticSoundAsset* ReacousticSound{nullptr};
	
};

// The map with all references to quickly be able to access the right audio data.
UCLASS(BlueprintType)
class UReacousticSoundAssociationMap : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = MeshToAudioMap)
	TArray<FMeshToAudioMapEntry> MeshMapEntries;
 
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = MaterialToAudioMap)
	TArray<FPhysicalMaterialToAudioMapEntry> PhysicalMaterialMapEntries;
};


