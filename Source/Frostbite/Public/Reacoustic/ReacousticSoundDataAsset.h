// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ReacousticSoundData.h"
#include "ReacousticSoundDataAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class SHOCKWORKS_API UReacousticSoundDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ReacousticSoundData)
	TArray<FReacousticSoundData> AudioData;
	
};
