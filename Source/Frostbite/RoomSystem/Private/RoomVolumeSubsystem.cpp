// // Copyright (c) 2022-present Barrelhouse
// Written by Tim Peeters
// This source code is part of the project Frostbite


#include "RoomVolumeSubsystem.h"


void URoomVolumeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
}

void URoomVolumeSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

#if WITH_EDITOR
void URoomVolumeSubsystem::PrintRegisteredRoomVolumes()
{
	for (int i = 0; i < RoomVolumes.Num(); i++) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, RoomVolumes[i]->GetFName().ToString() + "/n");
	}
}
#endif