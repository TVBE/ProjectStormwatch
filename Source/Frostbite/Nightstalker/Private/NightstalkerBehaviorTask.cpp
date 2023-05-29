// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#include "NightstalkerBehaviorTask.h"
#include "NightstalkerController.h"

DEFINE_LOG_CATEGORY_CLASS(UBTTask_NightstalkerBase, LogNightstalkerBehavior);

EBTNodeResult::Type UBTTask_NightstalkerBase::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Controller = Cast<ANightstalkerController>(OwnerComp.GetAIOwner());

	if (!Controller)
	{
		return EBTNodeResult::Failed;
	}

	Nightstalker = Cast<ANightstalker>(Controller->GetPawn());

	if (!Nightstalker)
	{
		return EBTNodeResult::Failed;
	}
	
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}

void UBTTask_NightstalkerBase::LogText(const FString& Text)
{
	UE_LOG(LogNightstalkerBehavior, VeryVerbose, TEXT("%s"), *Text);
}
