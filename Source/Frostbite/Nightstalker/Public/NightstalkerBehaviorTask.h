// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlueprintBase.h"
#include "NightstalkerBehaviorTask.generated.h"

class ANightstalkerController;
class ANightstalker;

UCLASS(Abstract, Blueprintable)
class FROSTBITE_API UBTTask_NightstalkerBase : public UBTTask_BlueprintBase
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogNightstalkerBehavior, Log, All)

private:
	UPROPERTY()
	ANightstalkerController* Controller {nullptr};

	UPROPERTY()
	ANightstalker* Nightstalker {nullptr};

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override
	{
	}

protected:
	UFUNCTION(BlueprintCallable, Category = "Logging", Meta = (DevelopmentOnly))
	void LogText(const FString& Text);

	UFUNCTION(BlueprintPure, Meta = (CompactNodeTitle = "Controller"))
	FORCEINLINE ANightstalkerController* GetController() { return Controller; }
	
	UFUNCTION(BlueprintPure, Meta = (CompactNodeTitle = "Nightstalker"))
	FORCEINLINE ANightstalker* GetNightstalker() { return Nightstalker; }
};
