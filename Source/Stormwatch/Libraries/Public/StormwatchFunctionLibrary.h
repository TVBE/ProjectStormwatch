// Copyright (c) 2022-present Barrelhouse. All rights reserved.
// Written by Tim Verberne.

#pragma once

#include "CoreMinimal.h"
#include "ActorFunctionCaller.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "StormwatchFunctionLibrary.generated.h"

class APlayerCharacter;
UENUM(BlueprintType, Meta = (DisplayName = "Function Result"))
enum class EFunctionResult : uint8
{
	Successful					UMETA(DisplayName = "Successful"),
	Unsuccessful				UMETA(DisplayName = "Unsuccessful"),
};

/** The interaction type of an object. This enumeration is currently only used in the the FindObjectThatImplementsInterface function.*/
UENUM(BlueprintType, Meta = (DisplayName = "Stormwatch Interface Type"))
enum class EStormwatchInterfaceType : uint8
{
	InteractableObject			UMETA(DisplayName = "Interactable Object Interface"),
	UsableObject				UMETA(DisplayName = "Usable Object Interface"),
	GrabbableObject				UMETA(DisplayName = "Grabbable Object Interface"),
	DraggableObject				UMETA(DisplayName = "Draggable Object Interface"),
	InventoryObject				UMETA(DisplayName = "Inventory Object Interface"),
	PowerConsumer				UMETA(DisplayName = "Power Consumer Interface")
};

UCLASS()
class STORMWATCH_API UStormwatchFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Tries to find the first object that implements the specified interface. This object could be the actor itself, or any other UObject derived class.
	 *	@Param Successful Whether the function successfully found the object.
	 *	@Param Actor The actor to search in.
	 *	@Param Interface The interface to search for.
	 *	@Return The object that implements the interface, if any. This can be null, so perform a IsValid check.
	 */
	UFUNCTION(BlueprintCallable, Category = "Interface", Meta = (DisplayName = "Find Object That Implements Interface By Type", ExpandEnumAsExecs = "Result"))
	static UObject* SearchActorForObjectThatImplementsInterface(EFunctionResult& Result, AActor* Actor, EStormwatchInterfaceType Interface);

	/** Tries to execute the function of a ActorFunctionCaller struct.
	 *	@Param FunctionCaller The ActorFunctionCaller struct to use.
	 */
	UFUNCTION(BlueprintCallable, Category = "Actor Function Caller", Meta = (DisplayName = "Call Function On Actor"))
	static void CallFunctionOnActorUsingActorFunctionCaller(FActorFunctionCaller FunctionCaller);

	/** Returns the player character instance if it is already spawned in the world.
	 *	This function performs no casts and can safely be called every frame without performance repercussions. */
	UFUNCTION(BlueprintPure, Meta = (DisplayName = "Get Stormwatch Player Character",
		CompactNodeTitle = "Player Character", Keywords = "Get, Stormwatch, Player, Character",
		WorldContext = "WorldContextObject"))
	static APlayerCharacter* GetStormwatchPlayerCharacter(const UObject* WorldContextObject);

	/** Returns the player character controller instance if it is already spawned in the world.
	 *	This function performs no casts and can safely be called every frame without performance repercussions. */
	UFUNCTION(BlueprintPure, Meta = (DisplayName = "Get Stormwatch Player Controller", CompactNodeTitle = "Player Controller",
		Keywords = "Get, Stormwatch, Player, Character, Controller",
		WorldContext = "WorldContextObject"))
	static APlayerCharacterController* GetStormwatchPlayerCharacterController(const UObject* WorldContextObject);

	/** Returns the Nightstalker instance if it is already spawned in the world.
	 *	This function performs no casts and can safely be called every frame without performance repercussions. */
	UFUNCTION(BlueprintPure, Meta = (DisplayName = "Get Stormwatch Nightstalker", CompactNodeTitle = "Nightstalker",
		Keywords = "Get, Stormwatch, Nightstalker",
		WorldContext = "WorldContextObject"))
	static ANightstalker* GetStormwatchNightstalker(const UObject* WorldContextObject);
	

	/** Plays an auditory event at a location. */
	UFUNCTION(BlueprintCallable, Category = "Nightstalker Director", meta = (WorldContext = "WorldContextObject"))
	static void PlayAuditoryEventAtLocation(const UObject* WorldContextObject, const FAuditoryEvent& AuditoryEvent, const FVector& Location);
};


