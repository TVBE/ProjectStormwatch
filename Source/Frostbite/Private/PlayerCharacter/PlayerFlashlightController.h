// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerFlashlightController.generated.h"

/** UPlayerFlashlightController is an Actor Component responsible for controlling the player's flashlight. 
 *	This class provides a simple and convenient way for designers to customize the player's flashlight behavior.
 *	@Brief ActorComponent for controlling the player's flashlight.
 */
UCLASS(Blueprintable, ClassGroup=(PlayerCharacter), meta=(BlueprintSpawnableComponent) )
class UPlayerFlashlightController : public UActorComponent
{
	GENERATED_BODY()
	
private:
	/** Pointer to the PlayerCharacter this component is part of. */
	class APlayerCharacter* PlayerCharacter {nullptr};

	/** Alpha value for blending the flashlight rotation based on movement. */
	float MovementAlpha {0.f};
	
public:	
	// Sets default values for this component's properties
	UPlayerFlashlightController();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Enables or disables the flashlight. */
	UFUNCTION(BlueprintCallable, Category = Default, Meta = (DisplayName = "Set Flashlight Enabled"))
	void SetFlashlightEnabled(const bool Value);

	/** Returns whether the flashlight is enabled or not. */
	UFUNCTION(BlueprintPure, Category = Default, Meta = (DisplayName = "Is Flashlight Enabled"))
	bool IsFlashlightEnabled();

	/** Updates the movement alpha value. */
	UFUNCTION()
	void UpdateMovementAlpha(const float DeltaTime);

	/** Calculates the flashlight focus rotation. */
	UFUNCTION()
	FRotator GetFlashlightFocusRotation();

	/** Calculates the flashlight */
	UFUNCTION()
	FRotator GetFlashlightSwayRotation();
};
