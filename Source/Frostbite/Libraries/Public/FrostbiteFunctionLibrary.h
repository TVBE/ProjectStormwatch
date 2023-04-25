// Copyright Notice

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FrostbiteFunctionLibrary.generated.h"

class APlayerCharacter;
UENUM(BlueprintType, Meta = (DisplayName = "Function Result"))
enum class EFunctionResult : uint8
{
	Successful					UMETA(DisplayName = "Successful"),
	Unsuccessful				UMETA(DisplayName = "Unsuccessful"),
};

/** The interaction type of an object. This enumeration is currently only used in the the FindObjectThatImplementsInterface function.*/
UENUM(BlueprintType, Meta = (DisplayName = "Frostbite Interface Type"))
enum class EFrostbiteInterfaceType : uint8
{
	InteractableObject			UMETA(DisplayName = "Interactable Object Interface"),
	UsableObject				UMETA(DisplayName = "Usable Object Interface"),
	GrabbableObject				UMETA(DisplayName = "Grabbable Object Interface"),
	DraggableObject				UMETA(DisplayName = "Draggable Object Interface"),
	InventoryObject				UMETA(DisplayName = "Inventory Object Interface"),
	PowerConsumer				UMETA(DisplayName = "Power Consumer Interface"),
	Door						UMETA(DisplayName = "Door Interface")
};

UCLASS()
class FROSTBITE_API UFrostbiteFunctionLibrary : public UBlueprintFunctionLibrary
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
	static UObject* SearchActorForObjectThatImplementsInterface(EFunctionResult& Result, AActor* Actor, EFrostbiteInterfaceType Interface);

	/** Returns the player character instance if it is already spawned in the world.
	 *	This function performs no casts and can safely be called every frame without performance repercussions. */
	UFUNCTION(BlueprintPure, Category = "Frostbite", Meta = (DisplayName = "Get Frostbite Player Character",
		CompactNodeTitle = "Player Character", Keywords = "Get, Frostbite, Player, Character",
		WorldContext = "WorldContextObject", DefaultToSelf = "WorldContextObject", HideSelfPin))
	static APlayerCharacter* GetFrostbitePlayerCharacter(const UObject* WorldContextObject);

	/** Returns the player character controller instance if it is already spawned in the world.
	 *	This function performs no casts and can safely be called every frame without performance repercussions. */
	UFUNCTION(BlueprintPure, Category = "Frostbite", Meta = (DisplayName = "Get Frostbite Player Controller", CompactNodeTitle = "Player Controller",
		Keywords = "Get, Frostbite, Player, Character, Controller",
		WorldContext = "WorldContextObject", DefaultToSelf = "WorldContextObject", HideSelfPin))
	static APlayerCharacterController* GetFrostbitePlayerCharacterController(const UObject* WorldContextObject);
};


