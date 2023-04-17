// // Copyright (c) 2022-present Barrelhouse
// Written by Tim Peeters
// This source code is part of the project Frostbite
#pragma once

#include "CoreMinimal.h"
#include "RoomVolume.h"
#include "Subsystems/Subsystem.h"
#include "RoomVolumeSubsystem.generated.h"

/**
 * Managing class for the RoomSystem. Subsystem spawned by the GameInstance,
 * used for storing data about RoomVolumes and Heatmap data.
 */
UCLASS(ClassGroup = "RoomSystem")
class FROSTBITE_API URoomVolumeSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	TArray<ARoomVolume*> RoomVolumes;
	
private:
	IConsoleCommand* PrintRoomVolumesCmd;
	IConsoleCommand* ShowRoomVolumeHeatCmd;

	FTimerHandle RoomVolumeDebugTimer;
protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
public:
	UFUNCTION()
	void Exec_PrintRegisteredRoomVolumes();
	UFUNCTION()
	void Exec_ShowRoomVolumesHeatValue();
private:
	UFUNCTION()
	void DrawRoomVolumeDebugBox();
};
