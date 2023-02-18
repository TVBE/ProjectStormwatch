// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerVfxController.generated.h"

class APlayerCharacter;

/** UPlayerAudioController is an Actor Component responsible for managing all VFX specific to the player character. 
 *	This class provides a simple and convenient way for designers to customize the player's VFX implementation.
 *	@Brief ActorComponent for managing player visual effects.
 */
UCLASS(Blueprintable, ClassGroup=(PlayerCharacter), meta=(BlueprintSpawnableComponent) )
class UPlayerVfxController : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerVfxController();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void InitializeComponent() override;

	/** Pointer to the PlayerCharacter that owns this component. */
	UPROPERTY(BlueprintReadOnly, Category = "PlayerVFXController", Meta = (DisplayName = "Player Character"))
	APlayerCharacter* PlayerCharacter;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
