// Copyright 2023 Barrelhouse

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "RoomVolume.generated.h"

class APlayerCharacter;
class ANightstalker;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerEnterDelegate, APlayerCharacter*, PlayerCharacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerLeaveDelegate, APlayerCharacter*, PlayerCharacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNightstalkerEnterDelegate, ANightstalker*, Nightstalker);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNightstalkerLeaveDelegate, ANightstalker*, Nightstalker);

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = ("RoomSystem"), Category = "RoomSystem")
class ARoomVolume : public ATriggerBox
{
	GENERATED_BODY()

public:
	// DELEGATES
	/** The delegate that is called when the player enters the room volume. */
	FPlayerEnterDelegate OnPlayerEnter;

	/** The delegate that is called when the player leaves the room volume. */
	FPlayerLeaveDelegate OnPlayerLeave;

	/** The delegate that is called when the nightstalker enters the room volume. */
	FNightstalkerEnterDelegate OnNightstalkerEnter;

	/** The delegate that is called when the nightstalker leaves the room volume. */
	FNightstalkerLeaveDelegate OnNightstalkerLeave;
	
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "Actors", Meta = (DisplayName = "Adjacent rooms"))
	TArray<TSoftObjectPtr<ARoomVolume>> ConnectedRooms;

protected:
	/** Blueprint native event for when the player enters the room volume. */


};


	