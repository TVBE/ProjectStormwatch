// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne and Tim Peeters
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "RoomVolume.generated.h"

class APlayerCharacter;
class ANightstalker;

UENUM()
enum ERoomHeatEvent_Type
{
	UNDEFINED,
	AUDITORY,
	VISUAL,
};

/** Struct used to parse event data to the RoomVolume, which increases/decreases the HeatValue.*/
USTRUCT(BlueprintType)
struct FRoomHeatEvent
{
	GENERATED_USTRUCT_BODY()
	
	/** Defines what type of event this is. Is it triggered by audio, sight?*/
	UPROPERTY(BlueprintReadWrite, Category = "RoomData", Meta = (DisplayName = "EventType"))
	TEnumAsByte<ERoomHeatEvent_Type> EventType = ERoomHeatEvent_Type::UNDEFINED;

	/**Amount to increase/decrease the Room's Heat Value when triggered by this event.*/
	UPROPERTY(BlueprintReadWrite, Category = "RoomData", Meta = (DisplayName = "EventHeatValue"))
	float EventHeatValue = 5;

	/**Range at which this event is applicable, e.g. this auditory event can be heard for a range of 10m*/
	UPROPERTY(BlueprintReadWrite, Category = "RoomData", Meta = (DisplayName = "EventRange"))
	float EventRange = 10.0f;

	UPROPERTY(BlueprintReadOnly, Category = "RoomData", Meta = (DisplayName = "EventLocation"))
	FVector EventLocation = FVector::ZeroVector;
	
	UPROPERTY(BlueprintReadOnly, Category = "RoomData", Meta = (DisplayName = "RelevantRooms"))
	TArray<ARoomVolume*> RelevantRooms;
};

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
	/** Room's current 'Heat' (Range 0-100, 100=hot). Variable used by e.g. AI to determine relevancy of room as a travel target*/
	UPROPERTY(BlueprintGetter = GetHeatValue, Category = "RoomVolume|Heatmap", Meta = (DisplayName = "HeatValue"))
	float HeatValue = 0;
	
	/** Whether the room is currently lit or not. */
	UPROPERTY(BlueprintGetter = GetIsLit, Category = "RoomVolume|Lighting", Meta = (DisplayName = "Is Lit"))
	bool IsLit {false};

public:
	/** Adds Heat to the HeatValue, based on a FRoomHeatEvent.*/
	UFUNCTION(BlueprintCallable, Category = "RoomVolume|Heatmap", Meta = (DisplayName = "Add Room Heat"))
	void AddRoomHeat(const FRoomHeatEvent HeatEvent);
	void AddRoomHeat(const float HeatToAdd);
	
	/** Deducts Heat from the HeatValue, based on a FRoomHeatEvent.*/
	UFUNCTION(BlueprintCallable, Category = "RoomVolume|Heatmap", Meta = (DisplayName = "Deduct Room Heat"))
	void DeductRoomHeat(const FRoomHeatEvent HeatEvent);
	void DeductRoomHeat(const float HeatToDeduct);
	
	/** Set whether the room should be considered lit or not. */
	UFUNCTION(BlueprintCallable, Category = "RoomVolume|Lighting", Meta = (DisplayName = "Set Light Status"))
	void SetLightStatus(const bool Value);
	
protected:
	/*Used for subscribing and desubscribing Volumes to the RoomVolume Subsystem*/
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	
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
	/** Returns the room's current heat value (Range 0-100)**/
	UFUNCTION(BlueprintGetter, Category = "RoomVolume|Heatmap", Meta = (DisplayName = "Get HeatValue"))
	FORCEINLINE float GetHeatValue() const {return HeatValue; }
	
	/** Returns whether the room is currently lit. */
	UFUNCTION(BlueprintGetter, Category = "RoomVolume|Lighting", Meta = (DisplayName = "Get IsLit"))
	FORCEINLINE bool GetIsLit() const {return IsLit; }
	
};


	