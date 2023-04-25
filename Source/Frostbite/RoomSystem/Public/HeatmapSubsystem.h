// // Copyright (c) 2022-present Barrelhouse
// Written by Tim Peeters
// This source code is part of the project Frostbite
#pragma once

#include "CoreMinimal.h"
#include "RoomVolume.h"
#include "Subsystems/Subsystem.h"
#include "HeatmapSubsystem.generated.h"

/**
 * Managing class for the Heatmap System. Subsystem spawned by the GameInstance,
 * used for storing data about Nightstalker and Heatmap data.
 */

UCLASS(Blueprintable, ClassGroup = "Heatmap")
class FROSTBITE_API UHeatmapSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	TArray<ARoomVolume*> RoomVolumes;

	/**Rate (in sec) at which the room's heat decays over time.*/
	UPROPERTY(BlueprintReadWrite, Category = "HeatmapSystem", Meta = (DisplayName = "HeatamapDecayRate"))
	float HeatDecayRate = 0.5f;
	
private:
	IConsoleCommand* PrintRoomVolumesCmd;
	IConsoleCommand* ShowRoomVolumeHeatCmd;

	FTimerHandle HeatmapDebugTimer;
protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	FTimerHandle RoomDecayTimer;
	
public:
	UFUNCTION(BlueprintCallable, Category = "HeatmapSystem", Meta = (DisplayName = "TriggerHeatmapEvent"))
	void TriggerHeatmapEvent(FRoomHeatEvent HeatEvent,  TArray<AActor*> ObjectsToIgnore);
	
	UFUNCTION()
	void Exec_PrintRegisteredRoomVolumes();
	UFUNCTION()
	void Exec_ShowHeatValue();
	
private:
	UFUNCTION()
	void DrawRoomVolumeDebugBox();
	UFUNCTION()
	void DecayHeatmapValues();
};
