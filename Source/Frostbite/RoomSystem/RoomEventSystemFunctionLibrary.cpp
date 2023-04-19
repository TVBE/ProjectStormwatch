// // // Copyright (c) 2022-present Barrelhouse // Written by Tim Peeters // This source code is part of the project Frostbite

#include "RoomEventSystemFunctionLibrary.h"

#include "RoomVolumeSubsystem.h"
#include "Kismet/GameplayStatics.h"

void URoomEventSystemFunctionLibrary::PlaySoundAtLocation_RoomEvent(const UObject* WorldContextObject, FRoomHeatEvent HeatEvent,
	USoundBase* Sound, FTransform SoundTransform, float VolumeMultiplier, float PitchMultiplier, float StartTime,
	USoundAttenuation* AttenuationSettings, USoundConcurrency* ConcurrencySettings, const AActor* OwningActor)
{
	const TArray<AActor*> IgnoredActors;
	URoomVolumeSubsystem* RoomSubSystem = WorldContextObject->GetWorld()->GetGameInstance()->GetSubsystem<URoomVolumeSubsystem>();
	RoomSubSystem->TriggerRoomEvent(HeatEvent, IgnoredActors);
	
	UGameplayStatics::PlaySoundAtLocation(WorldContextObject, Sound, SoundTransform.GetLocation(), SoundTransform.Rotator(),
	VolumeMultiplier, PitchMultiplier, StartTime,
	AttenuationSettings, ConcurrencySettings, OwningActor);
}
