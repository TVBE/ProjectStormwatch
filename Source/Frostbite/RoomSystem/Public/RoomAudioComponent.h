// Copyright (c) 2022-present Barrelhouse. All rights reserved.
// Written by Tim Verberne.

#pragma once

#include "CoreMinimal.h"
#include "RoomComponent.h"
#include "Components/ActorComponent.h"
#include "RoomAudioComponent.generated.h"


UCLASS(Blueprintable, BlueprintType, ClassGroup = "Room System", Meta = (BlueprintSpawnableComponent))
class FROSTBITE_API URoomAudioComponent : public URoomComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Reverb", Meta = (DisplayName = "Reverb"))
	USubmixEffectConvolutionReverbPreset* ReverbConvolutionReverbPreset {nullptr};

	UPROPERTY(EditDefaultsOnly, Category = "Reverb", Meta = (DisplayName = "Reverb"))
	UDataTable* ReverbTypes {nullptr};

public:	
	URoomAudioComponent();

protected:
	virtual void BeginPlay() override;

	virtual void ConstructComponent() override;

private:
	UFUNCTION(BlueprintCallable)
	USubmixEffectConvolutionReverbPreset* FindMatchingImpulseResponseFromDataTable(UDataTable* DataTable, const float Volume, const float ShapeRatio, const float Reflectivity);
};

USTRUCT(BlueprintType)
struct FRoomReverbSettings : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Reverb", Meta = (DisplayName = "Impulse Response A"))
	USubmixEffectConvolutionReverbPreset* ImpulseResponseA {nullptr};

	UPROPERTY(EditAnywhere, Category = "Reverb", Meta = (DisplayName = "Impulse Response B"))
	USubmixEffectConvolutionReverbPreset* ImpulseResponseB {nullptr};
	
	UPROPERTY(EditAnywhere, Category = "Room", Meta = (DisplayName = "Volume", ForceUnits = "m3", ClampMin = "0", ClampMax = "500", UIMin = "0", UIMax ="500"))
	float RoomVolume {0.0f};

	UPROPERTY(EditAnywhere, Category = "Room", Meta = (ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax ="1"))
	float ShapeRatio {0.0f};

	UPROPERTY(EditAnywhere, Category = "Room", Meta = (ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax ="1"))
	float Reflectivity {0.5f};
};

UCLASS(BlueprintType)
class FROSTBITE_API URoomReverbDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (ShowOnlyInnerProperties))
	FRoomReverbSettings RoomReverbSettings;
};
