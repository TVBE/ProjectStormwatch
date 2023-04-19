// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#include "RoomVolume.h"
#include "Nightstalker.h"
#include "PlayerCharacter.h"
#include "LogCategories.h"
#include "RoomVolumeSubsystem.h"

void ARoomVolume::BeginPlay()
{
	Super::BeginPlay();

	UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	URoomVolumeSubsystem* RoomVolumeSubsystem = GameInstance->GetSubsystem<URoomVolumeSubsystem>();

	if(GameInstance)
		if(RoomVolumeSubsystem)
			if(!RoomVolumeSubsystem->RoomVolumes.Contains(this)) { RoomVolumeSubsystem->RoomVolumes.Add(this);}
}

void ARoomVolume::Destroyed()
{
	UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	URoomVolumeSubsystem* RoomVolumeSubsystem = GameInstance->GetSubsystem<URoomVolumeSubsystem>();
	
	if(RoomVolumeSubsystem)
		if(RoomVolumeSubsystem->RoomVolumes.Contains(this)) { RoomVolumeSubsystem->RoomVolumes.Remove(this);}
	
	Super::Destroyed();
}

void ARoomVolume::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	if (APlayerCharacter* PlayerCharacter {Cast<APlayerCharacter>(OtherActor)})
	{
		OnPlayerEnter.Broadcast(PlayerCharacter);
		EventOnPlayerEnter(PlayerCharacter);

		UE_LOG(LogRoomVolume, Verbose, TEXT("Player has entered room %s."), *this->GetName())
		
		return;
	}
	if (ANightstalker* Nightstalker {Cast<ANightstalker>(OtherActor)})
	{
		OnNightstalkerEnter.Broadcast(Nightstalker);
		EventOnNightstalkerEnter(Nightstalker);

		UE_LOG(LogRoomVolume, Verbose, TEXT("Nightstalker has entered room %s."), *this->GetName())
	}
}

void ARoomVolume::AddRoomHeat(const FRoomHeatEvent HeatEvent)
{
	float NewHeatVal = (HeatValue + HeatEvent.EventHeatValue);
	if(NewHeatVal > 100.0) {NewHeatVal = 100.0f;}
	else if(NewHeatVal < 0) {NewHeatVal = 0.0f;}

	HeatValue = NewHeatVal;
}

void ARoomVolume::AddRoomHeat(const float HeatToAdd)
{
	float NewHeatVal = (HeatValue + HeatToAdd);
	if(NewHeatVal > 100.0) {NewHeatVal = 100.0f;}
	else if(NewHeatVal < 0) {NewHeatVal = 0.0f;}

	HeatValue = NewHeatVal;
}

void ARoomVolume::DeductRoomHeat(const FRoomHeatEvent HeatEvent)
{
	float NewHeatVal = (HeatValue - HeatEvent.EventHeatValue);
	if(NewHeatVal > 100.0) {NewHeatVal = 100.0f;}
	else if(NewHeatVal < 0) {NewHeatVal = 0.0f;}

	HeatValue = NewHeatVal;
}

void ARoomVolume::DeductRoomHeat(const float HeatToDeduct)
{
	float NewHeatVal = (HeatValue - HeatToDeduct);
	if(NewHeatVal > 100.0f) {NewHeatVal = 100.0f;}
	else if(NewHeatVal < 0) {NewHeatVal = 0.0f;}

	HeatValue = NewHeatVal;
}

void ARoomVolume::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);
	if (APlayerCharacter* PlayerCharacter {Cast<APlayerCharacter>(OtherActor)})
	{
		OnPlayerLeave.Broadcast(PlayerCharacter);
		EventOnPlayerLeave(PlayerCharacter);

		UE_LOG(LogRoomVolume, Verbose, TEXT("Player has left room %s."), *this->GetName())
		
		return;
	}
	if (ANightstalker* Nightstalker {Cast<ANightstalker>(OtherActor)})
	{
		OnNightstalkerLeave.Broadcast(Nightstalker);
		EventOnNightstalkerLeave(Nightstalker);

		UE_LOG(LogRoomVolume, Verbose, TEXT("Nightstalker has left room %s."), *this->GetName())
	}
}

// BLUEPRINT NATIVE EVENTS
void ARoomVolume::EventOnPlayerEnter_Implementation(APlayerCharacter* PlayerCharacter)
{
}
void ARoomVolume::EventOnPlayerLeave_Implementation(APlayerCharacter* PlayerCharacter)
{
}
void ARoomVolume::EventOnNightstalkerEnter_Implementation(ANightstalker* Nightstalker)
{
}
void ARoomVolume::EventOnNightstalkerLeave_Implementation(ANightstalker* Nightstalker)
{
}


