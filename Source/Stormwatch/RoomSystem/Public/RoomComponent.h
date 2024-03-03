// Copyright (c) 2022-present Barrelhouse. All rights reserved.
// Written by Tim Verberne.

#pragma once

#include "CoreMinimal.h"
#include "RoomComponent.generated.h"

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "Room System")
class STORMWATCH_API URoomComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URoomComponent();

	virtual void ConstructComponent() {};
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void HandlePawnEnter(APawn* Pawn);
	virtual void HandlePawnLeave(APawn* Pawn);
	
	UFUNCTION(BlueprintNativeEvent, Category = "Events", Meta = (DisplayName = "On Pawn Enter"))
	void EventPawnEnter(APawn* Pawn);

	UFUNCTION(BlueprintNativeEvent, Category = "Events", Meta = (DisplayName = "On Pawn Leave"))
	void EventPawnLeave(APawn* Pawn);
};