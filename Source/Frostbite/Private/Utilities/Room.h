// Copyright 2023 Barrelhouse

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "Room.generated.h"

/**
 * 
 */
UCLASS()
class ARoom : public ATriggerBox
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "Actors", Meta = (DisplayName = "Adjacent rooms"))
	TArray<TSoftObjectPtr<ARoom>> AjacentRooms;

	UFUNCTION(BlueprintCallable, Category = "Actors", Meta = (DisplayName = "Set box extent"))
	void SetBoxExtent(FVector3f BoxExtent);

};


	