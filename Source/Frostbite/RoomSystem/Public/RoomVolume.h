// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "RoomVolume.generated.h"

class APlayerCharacter;
class ANightstalker;

/** Struct for defining connected rooms. */
USTRUCT(BlueprintType)
struct FRoomPathData
{
	GENERATED_USTRUCT_BODY()

	/** Soft object pointer to the room. */
	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category = "RoomPathData", Meta = (DisplayName = "Room"))
	TSoftObjectPtr<ARoomVolume> Room {nullptr};

	/** Defines whether the nightstalker can enter this room this way. */
	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category = "RoomPathData", Meta = (DisplayName = "Can Nightstalker Take Path"))
	bool CanNightstalkerTakePath {true};
	
	/** Constructor with default values. */
	FRoomPathData()
	{
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerEnterDelegate, APlayerCharacter*, PlayerCharacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerLeaveDelegate, APlayerCharacter*, PlayerCharacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNightstalkerEnterDelegate, ANightstalker*, Nightstalker);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNightstalkerLeaveDelegate, ANightstalker*, Nightstalker);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLuminosityChanged, bool, IsLit);

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "RoomSystem")
class ARoomVolume : public ATriggerBox
{
	GENERATED_BODY()

public:
	// DELEGATES
	/** Delegate that is called when the player enters the room volume. */
	FPlayerEnterDelegate OnPlayerEnter;

	/** Delegate that is called when the player leaves the room volume. */
	FPlayerLeaveDelegate OnPlayerLeave;

	/** Delegate that is called when the nightstalker enters the room volume. */
	FNightstalkerEnterDelegate OnNightstalkerEnter;

	/** Delegate that is called when the nightstalker leaves the room volume. */
	FNightstalkerLeaveDelegate OnNightstalkerLeave;

	/** Delegate that is called when the luminosity of the room has changed. */
	FLuminosityChanged OnLuminosityChanged;
	
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "Actors", Meta = (DisplayName = "Adjacent rooms"))
	TArray<TSoftObjectPtr<ARoomVolume>> ConnectedRooms;

private:
	/** Whether the room is currently lit or not. */
	UPROPERTY(BlueprintGetter =GetIsLit, Category = "RoomVolume", Meta = (DisplayName = "Is Lit"))
	bool IsLit {false};

public:
	/** Set whether the room should be considered lit or not. */
	UFUNCTION(BlueprintCallable, Category = "RoomVolume", Meta = (DisplayName = "Set Light Status"))
	void SetLightStatus(const bool Value);
	
protected:
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;
	
	/** Blueprint native event for when the player enters the room volume.
	 *	@Param PlayerCharacter Pointer to the player character that entered the room volume.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "RoomVolume", Meta = (DisplayName = "On Player Enter", BlueprintProtected))
	void EventOnPlayerEnter(APlayerCharacter* PlayerCharacter);
	
	/** Blueprint native event for when the player leaves the room volume.
	 *	@Param PlayerCharacter Pointer to the player character that left the room volume.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "RoomVolume", Meta = (DisplayName = "On Player Leave", BlueprintProtected))
	void EventOnPlayerLeave(APlayerCharacter* PlayerCharacter);
	
	/** Blueprint native event for when the nightstalker enters the room volume.
	 *	@Param PlayerCharacter Pointer to the nightstalker that entered the room volume.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "RoomVolume", Meta = (DisplayName = "On Nightstalker Enter", BlueprintProtected))
	void EventOnNightstalkerEnter(ANightstalker* Nightstalker);
	
	/** Blueprint native event for when the nightstalker enters the room volume.
	 *	@Param PlayerCharacter Pointer to the nightstalker that left the room volume
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "RoomVolume", Meta = (DisplayName = "On Nightstalker Leave", BlueprintProtected))
	void EventOnNightstalkerLeave(ANightstalker* Nightstalker);

public:
	/** Returns whether the room is currently lit. */
	UFUNCTION(BlueprintGetter, Category = "RoomVolume", Meta = (DisplayName = "Is Lit"))
	FORCEINLINE bool GetIsLit() const {return IsLit; }
	

};


	