// Copyright 2023 Barrelhouse

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Nightstalker.generated.h"

UCLASS(Abstract, Blueprintable, BlueprintType, NotPlaceable, ClassGroup = (Nightstalker))
class ANightstalker : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ANightstalker();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
