// // // Copyright (c) 2022-present Barrelhouse // Written by Tim Peeters // This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "RoomVolume.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RoomEventSystemFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class FROSTBITE_API URoomEventSystemFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()


public:
	UFUNCTION(BlueprintCallable, Category = "RoomEvent", meta=(DisplayName= "PlaySoundAtLocation RoomEvent", WorldContext="WorldContextObject", AdvancedDisplay = "3", UnsafeDuringActorConstruction = "true", Keywords = "play"))
	static void PlaySoundAtLocation_RoomEvent(const UObject* WorldContextObject, FRoomHeatEvent HeatEvent, USoundBase* Sound, FTransform SoundTransform,
	float VolumeMultiplier, float PitchMultiplier, float StartTime, USoundAttenuation* AttenuationSettings,
	USoundConcurrency* ConcurrencySettings, const AActor* OwningActor);
};
