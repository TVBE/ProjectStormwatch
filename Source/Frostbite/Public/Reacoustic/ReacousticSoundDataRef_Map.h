// Copyright 2023 Nino Saglia & Tim Verberne

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ReacousticSoundDataRef_Map.generated.h"


/**
 * 
 */

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

UCLASS(BlueprintType)
class SHOCKWORKS_API UReacousticSoundDataRef_Map : public UDataAsset
{
 GENERATED_BODY()

public:
 UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = MeshToAudioMap)
 TArray<FMeshToAudioMapEntry> MeshMapEntries;
 
 UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = MaterialToAudioMap)
 TArray<FPhysicalMaterialToAudioMapEntry> PhysicalMaterialMapEntries;
};






