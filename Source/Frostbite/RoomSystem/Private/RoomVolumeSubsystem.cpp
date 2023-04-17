// // Copyright (c) 2022-present Barrelhouse
// Written by Tim Peeters
// This source code is part of the project Frostbite


#include "RoomVolumeSubsystem.h"


void URoomVolumeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	PrintRoomVolumesCmd = IConsoleManager::Get().RegisterConsoleCommand(TEXT("PrintRegisteredRoomVolumes"),
	                                                                    TEXT(
		                                                                    "Prints all room volumes in the current level, "
		                                                                    "which are registered to the RoomVolumeSubsystem"),
	                                                                    FConsoleCommandDelegate::CreateUObject(
		                                                                    this,
		                                                                    &URoomVolumeSubsystem::Exec_PrintRegisteredRoomVolumes),
	                                                                    ECVF_Default);

	ShowRoomVolumeHeatCmd = IConsoleManager::Get().RegisterConsoleCommand(TEXT("ShowRoomVolumeHeat"),
	                                                                      TEXT(
		                                                                      "Shows heat for all RoomVolumes, by drawing debug boxes."),
	                                                                      FConsoleCommandDelegate::CreateUObject(
		                                                                      this,
		                                                                      &URoomVolumeSubsystem::Exec_ShowRoomVolumesHeatValue),
	                                                                      ECVF_Default);
}

void URoomVolumeSubsystem::Deinitialize()
{
	IConsoleManager::Get().UnregisterConsoleObject(PrintRoomVolumesCmd);
	IConsoleManager::Get().UnregisterConsoleObject(ShowRoomVolumeHeatCmd);
	Super::Deinitialize();
}

void URoomVolumeSubsystem::Exec_PrintRegisteredRoomVolumes()
{
	if (RoomVolumes.Num() > 0 && GEngine)
	{
		for (int i = 0; i < RoomVolumes.Num(); i++)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, RoomVolumes[i]->GetFName().ToString());
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "None Registered");
	}
}

void URoomVolumeSubsystem::Exec_ShowRoomVolumesHeatValue()
{
	if (GetGameInstance()->GetTimerManager().IsTimerActive(RoomVolumeDebugTimer))
	{
		GetGameInstance()->GetTimerManager().PauseTimer(RoomVolumeDebugTimer);
	}
	else
	{
		if (!GetGameInstance()->GetTimerManager().TimerExists(RoomVolumeDebugTimer))
		{
			GetGameInstance()->GetTimerManager().SetTimer(RoomVolumeDebugTimer, this,
			                                              &URoomVolumeSubsystem::DrawRoomVolumeDebugBox,
			                                              0.1f, true, 0.1f);
		}
		GetGameInstance()->GetTimerManager().UnPauseTimer(RoomVolumeDebugTimer);
	}
}

void URoomVolumeSubsystem::DrawRoomVolumeDebugBox()
{
	if (RoomVolumes.Num() > 0)
	{
		for (int i = 0; i < RoomVolumes.Num(); i++)
		{
			FVector RoomOrigin;
			FVector RoomBoundsExtent;
			RoomVolumes[i]->GetActorBounds(false, RoomOrigin, RoomBoundsExtent);
			
			//Start at 125, based on color balancing.
			const float HeatValueScaled = 125.0f * (RoomVolumes[i]->GetHeatValue() * 0.01f); 
			FColor HeatColor = FColor(100 + HeatValueScaled, 125 - HeatValueScaled, 0, 100);
			
			DrawDebugSolidBox(GetWorld(), RoomOrigin, RoomBoundsExtent * 0.99f, HeatColor, false, 0.1f, 0);
			
			DrawDebugString(GetWorld(), FVector(RoomOrigin.X, RoomOrigin.Y, RoomOrigin.Z - RoomBoundsExtent.Z)
				, FString::FromInt(RoomVolumes[i]->GetHeatValue()), NULL,
				FColor::White, 0.1f, false, 1);
		}
	}
}
