// Copyright 2023 Barrelhouse

#pragma once

#include "CoreMinimal.h"
#include "UsableObjectInterface"
#include "Components/ActorComponent.h"
#include "ActorInteractionComponent.generated.h"


UCLASS(Blueprintable, BlueprintType, Placeable, ClassGroup = (Interaction), Meta = (BlueprintSpawnableComponent) )
class UActorInteractionComponent : public UActorComponent, public IUsableObject
{
	GENERATED_BODY()

private:
	/** The Offset of the interaction widget. */
	UPROPERTY(EditInstanceOnly, Category = "Interaction", Meta = (DisplayName = "Widget Offset"))
	FVector InteractionWidgetOffset {FVector()};

public:	
	UActorInteractionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
