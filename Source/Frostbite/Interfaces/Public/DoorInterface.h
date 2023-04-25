// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "DoorInterface.generated.h"

UENUM(BlueprintType)
enum class EDoorState : uint8
{
    Open        UMETA(DisplayName = "Open"), 
    Closed      UMETA(DisplayName = "Closed"),
    Opening     UMETA(DisplayName = "Opening"),
    Closing     UMETA(DisplayName = "Closing"),
};

UENUM(BlueprintType)
enum class EDoorAction : uint8
{
    Open        UMETA(DisplayName = "Open"), 
    Close      UMETA(DisplayName = "Close"),
};

UINTERFACE(Blueprintable, Meta = (DisplayName = "Door Interface"))
class UDoor : public UInterface
{
    GENERATED_BODY()
};

class IDoor
{
    GENERATED_BODY()

public:
    /** Attempts to open the door. */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Door", Meta = (DisplayName = "Open"))
    bool Open(const AActor* Initiator);

    /** Attempts to close the door. */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Door", Meta = (DisplayName = "Close"))
    bool Close(const AActor* Initiator);
    
    /** Returns the door's state. */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Door", Meta = (DisplayName = "Get Door State"))
    EDoorState GetDoorState() const;
};