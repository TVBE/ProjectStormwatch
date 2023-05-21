// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "RoomComponent.generated.h"

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "Room System")
class FROSTBITE_API URoomComponent : public UActorComponent
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