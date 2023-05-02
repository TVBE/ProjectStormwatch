// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "RoomVolume.generated.h"

class APlayerCharacter;
class ANightstalker;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPawnEnterDelegate, APawn*, Pawn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnpawnLeaveDelegate, APawn*, Pawn);

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "Room System")
class ARoomVolume : public ATriggerBox
{
	GENERATED_BODY()

public:
	/** Delegate that is called when the player enters the room volume. */
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnPawnEnterDelegate OnPawnEnter;

	/** Delegate that is called when the player leaves the room volume. */
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnpawnLeaveDelegate OnPawnLeave;

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Volume", Meta = (ForceUnits = "m3"))
	float Volume {0.0f};

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Volume")
	float RoomRatio {0.0f};

private:
	UPROPERTY(BlueprintGetter = GetOverlappingPawns)
	TArray<APawn*> OverlappingPawns;

public:
	ARoomVolume();

protected:
	virtual void PostInitProperties() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

public:
	UFUNCTION(BlueprintGetter)
	FORCEINLINE TArray<APawn*> GetOverlappingPawns() const { return OverlappingPawns; }
};


	