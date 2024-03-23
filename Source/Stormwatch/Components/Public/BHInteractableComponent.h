// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "Components/SceneComponent.h"
#include "BHInteractableObjectInterface.h"
#include "Misc/Timespan.h"
#include "BHInteractableComponent.generated.h"

struct FBHInteractionEvent
{
	float Duration() const;
	bool HasStarted() const;
	bool HasFinished() const;
	
	TOptional<FDateTime> StartTime = FDateTime::Now();
	TOptional<FDateTime> EndTime;
};

UCLASS(Abstract, BlueprintType, ClassGroup = "Barrelhouse")
class STORMWATCH_API UBHInteractableComponent : public USceneComponent, public IBHInteractableObject  
{
	GENERATED_BODY()
	
public:
	//~ Begin BHInteractableObject interface.
	FVector GetInteractionWidgetLocation_Implementation() const override;
	//~ End BHInteractableObject interface.
	
	UFUNCTION(BlueprintPure, Category = "BHInteractionComponent")
	bool IsBeingInteractedWith() const;

	UFUNCTION(BlueprintPure, Category = "BHInteractionComponent")
	bool HasEverBeenInteractedWith() const;

	UFUNCTION(BlueprintPure, Category = "BHInteractionComponent")
	float GetDurationOfCurrentInteraction() const;
	
	UFUNCTION(BlueprintPure, Category = "BHInteractionComponent")
	float GetTimeSinceLastInteraction() const;

	UFUNCTION(BlueprintPure, Category = "BHInteractionComponent")
	float GetDurationOfLastInteraction() const;
	
	UFUNCTION(BlueprintPure, Category = "BHInteractionComponent")
	int32 GetInteractionCount() const;

	TOptional<FBHInteractionEvent> GetCurrentInteraction() const;
	TOptional<FBHInteractionEvent> GetLastInteraction() const;

protected:
	void OnRegister() override;

	virtual void OnBeginInteraction();
	virtual void OnEndInteraction();

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, Category = "Editor")
	bool bAllowOnlyOneInstancePerActor = false;
#endif

private:
	TOptional<FBHInteractionEvent> CurrentInteraction;
	TOptional<FBHInteractionEvent> LastInteraction;
	int32 InteractionCount = 0;
};