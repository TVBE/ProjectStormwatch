// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "BHInteractableObjectInterface.h"

#include "Components/SceneComponent.h"

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
	virtual bool CanBeInteractedWith_Implementation() const override;
	virtual FVector GetInteractionLocation_Implementation() const override;
	virtual int32 GetInteractionPriority_Implementation() const override;
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

	virtual void BeginInteraction();
	virtual void EndInteraction();

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, Category = "Editor")
	bool bAllowOnlyOneInstancePerActor = false;
#endif

private:
	TOptional<FBHInteractionEvent> CurrentInteraction;
	TOptional<FBHInteractionEvent> LastInteraction;
	int32 InteractionCount = 0;
};
