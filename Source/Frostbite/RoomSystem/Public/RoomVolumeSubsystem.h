// // Copyright (c) 2022-present Barrelhouse
// Written by Tim Peeters
// This source code is part of the project Frostbite
#pragma once

#include "CoreMinimal.h"
#include "RoomVolume.h"
#include "Subsystems/Subsystem.h"
#include "RoomVolumeSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class FROSTBITE_API URoomVolumeSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = "RoomVolume Subsystem", Meta = (DisplayName = "RoomVolumes"))
	TArray<ARoomVolume*> RoomVolumes;
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

#if WITH_EDITOR
	UFUNCTION(Exec, Category="ConsoleCommands")
	void PrintRegisteredRoomVolumes();
#endif
};
