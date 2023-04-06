// Copyright 2023 Barrelhouse

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerInventoryComponent.generated.h"

class UPlayerInteractionComponent;

UCLASS(Blueprintable, BlueprintType, ClassGroup = "PlayerCharacter", Meta = (BlueprintSpawnableComponent,
	DisplayName = "Player Inventory Component"))
class FROSTBITE_API UPlayerInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	/** Pointer to the interaction component of the player. */
	UPROPERTY()
	UPlayerInteractionComponent* InteractionComponent;

public:	
	UPlayerInventoryComponent();

	/** Checks if an actor or one of its components implements the IInventoryObject interface.
	*	Returns the first UObject that implements the interface that it finds. */
	UFUNCTION(BlueprintPure, Category = "PlayerInventoryComponent", Meta = (DisplayName = "Find Inventory Object"))
	UObject* FindInventoryObject(AActor* Actor) const;

protected:
	virtual void BeginPlay() override;

private:
	/** Called when the interaction component has detected a new interactable actor in front of the player. */
	UFUNCTION()
	void HandleInteractableActorChanged(AActor* InteractableActor);

	/** Tries to find a component that implements the IInventoryObject interface in a specified actor.*/
	UFUNCTION()
	UActorComponent* FindInventoryComponent(const AActor* Actor) const;
};
