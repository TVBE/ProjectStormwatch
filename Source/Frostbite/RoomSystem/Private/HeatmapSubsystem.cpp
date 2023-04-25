// // Copyright (c) 2022-present Barrelhouse
// Written by Tim Peeters
// This source code is part of the project Frostbite


#include "HeatmapSubsystem.h"

#include "Animation/AnimInstanceProxy.h"
#include "Kismet/KismetSystemLibrary.h"


void UHeatmapSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	//Set Decay Timer
	GetGameInstance()->GetTimerManager().SetTimer(RoomDecayTimer, this, &UHeatmapSubsystem::DecayHeatmapValues, HeatDecayRate, true, -1);

	/*---------------Register Heatmap Console Commands-----------------*/
	//Don't forget to unregister in deinit!!!
	PrintRoomVolumesCmd = IConsoleManager::Get().RegisterConsoleCommand(TEXT("PrintRegisteredRoomVolumes"),
	                                                                    TEXT(
		                                                                    "Prints all room volumes in the current level, "
		                                                                    "which are registered to the RoomVolumeSubsystem"),
	                                                                    FConsoleCommandDelegate::CreateUObject(
		                                                                    this,
		                                                                    &UHeatmapSubsystem::Exec_PrintRegisteredRoomVolumes),
	                                                                    ECVF_Default);

	ShowRoomVolumeHeatCmd = IConsoleManager::Get().RegisterConsoleCommand(TEXT("ShowRoomVolumeHeat"),
	                                                                      TEXT(
		                                                                      "Shows heat for all RoomVolumes, by drawing debug boxes."),
	                                                                      FConsoleCommandDelegate::CreateUObject(
		                                                                      this,
		                                                                      &UHeatmapSubsystem::Exec_ShowHeatValue),
	                                                                      ECVF_Default);
	/*-----------------------------------------------------------------*/
}

void UHeatmapSubsystem::Deinitialize()
{
	GetGameInstance()->GetTimerManager().ClearTimer(RoomDecayTimer);

	//Unregister Heatmap Console Commands
	IConsoleManager::Get().UnregisterConsoleObject(PrintRoomVolumesCmd);
	IConsoleManager::Get().UnregisterConsoleObject(ShowRoomVolumeHeatCmd);
	Super::Deinitialize();
}

void UHeatmapSubsystem::TriggerHeatmapEvent(FRoomHeatEvent HeatEvent, TArray<AActor*> ObjectsToIgnore)
{
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesArray;
	ObjectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Visibility));

	TArray<FHitResult> HitResults;
	
	UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), HeatEvent.EventLocation, HeatEvent.EventLocation, HeatEvent.EventRange, ObjectTypesArray, false, ObjectsToIgnore,
		EDrawDebugTrace::None, HitResults, true, FLinearColor::Green, FLinearColor::Red, 5.0f);

	for (FHitResult HitObject : HitResults)
	{
		ARoomVolume* HitRoom = Cast<ARoomVolume>(HitObject.GetActor());

		/**Temp calculation for falloff*/
		float HeatToAdd = HeatEvent.EventHeatValue * (1 - ((HeatEvent.EventLocation - HitObject.GetActor()->GetActorLocation()).Length() / HeatEvent.EventRange));
		
		if(HitRoom){HitRoom->AddRoomHeat(HeatToAdd);}
	}
}

void UHeatmapSubsystem::Exec_PrintRegisteredRoomVolumes()
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

void UHeatmapSubsystem::Exec_ShowHeatValue()
{
	if (GetGameInstance()->GetTimerManager().IsTimerActive(HeatmapDebugTimer))
	{
		GetGameInstance()->GetTimerManager().PauseTimer(HeatmapDebugTimer);
	}
	else
	{
		if (!GetGameInstance()->GetTimerManager().TimerExists(HeatmapDebugTimer))
		{
			GetGameInstance()->GetTimerManager().SetTimer(HeatmapDebugTimer, this,
			                                              &UHeatmapSubsystem::DrawRoomVolumeDebugBox,
			                                              0.1f, true, 0.1f);
		}
		GetGameInstance()->GetTimerManager().UnPauseTimer(HeatmapDebugTimer);
	}
}

void UHeatmapSubsystem::DrawRoomVolumeDebugBox()
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

void UHeatmapSubsystem::DecayHeatmapValues()
{
	if(RoomVolumes.IsEmpty()) return;
	
	for(auto Room : RoomVolumes)
	{
		if(Room->GetHeatValue() > 0.0f){Room->DeductRoomHeat(1.0f);}
	}
}
