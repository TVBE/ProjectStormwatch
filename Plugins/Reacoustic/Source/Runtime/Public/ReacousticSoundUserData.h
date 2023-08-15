// Copyright Notice

#pragma once

#include "CoreMinimal.h"
#include "ReacousticSoundAsset.h"
#include "Engine/AssetUserData.h"
#include "ReacousticSoundUserData.generated.h"

/**
 * 
 */
UCLASS()
class REACOUSTIC_API UReacousticSoundUserData : public UAssetUserData
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reacoustic")
	UReacousticSoundAsset* ReacousticSound;
};
