// Copyright 2023 Barrelhouse

#pragma once

#include "CoreMinimal.h"
#include "InteractableObjectInterface.h"
#include "Components/ActorComponent.h"
#include "ActorInteractionComponent.generated.h"


UCLASS(Blueprintable, BlueprintType, Placeable, ClassGroup = (Interaction), Meta = (BlueprintSpawnableComponent) )
class UActorInteractionComponent : public UActorComponent, public IInteractableObject
{
	GENERATED_BODY()

private:
	/** The interaction type of this interactable static mesh actor. */
	UPROPERTY(EditAnywhere, Category = "Interaction", Meta = (DisplayName = "Type"))
	EInteractionType InteractionType {EInteractionType::Grabbable};

	/** The interaction trigger type of this interactable static mesh actor. */
	UPROPERTY(EditAnywhere, Category = "Interaction", Meta = (DisplayName = "Trigger"))
	EInteractionTriggerType InteractionTriggerType {EInteractionTriggerType::SinglePress};

	/** The interaction trigger type of this interactable static mesh actor. */
	UPROPERTY(EditAnywhere, Category = "Interaction", Meta = (DisplayName = "Hands"))
	EInteractionHandType InteractionHandType {EInteractionHandType::OneHanded};

	/** The Offset of the interaction widget. */
	UPROPERTY(EditInstanceOnly, Category = "Interaction", Meta = (DisplayName = "Widget Offset"))
	FVector InteractionWidgetOffset {FVector()};

public:	
	UActorInteractionComponent();

	FORCEINLINE bool Use_Implementation(const AActor* Interactor) override { return false; }
	FORCEINLINE bool Disuse_Implementation(const AActor* Interactor) override { return false; }
	FORCEINLINE EInteractionType GetInteractionType_Implementation() const override { return InteractionType; }
	FORCEINLINE EInteractionTriggerType GetInteractionTriggerType_Implementation() const override { return InteractionTriggerType; }
	FORCEINLINE EInteractionHandType GetInteractionHandType_Implementation() const override { return InteractionHandType; }
	FORCEINLINE FVector GetInteractionWidgetOffset_Implementation() const override { return InteractionWidgetOffset; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
