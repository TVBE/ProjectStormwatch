// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerFlashlightController.generated.h"

enum class EPlayerGroundMovementType : uint8;

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
	UPROPERTY(BlueprintReadOnly, Category = Default, Meta = (DisplayName = "Movement Alpha", AllowPrivateAccess = "true"))
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
	bool IsFlashlightEnabled() const;

	/** Updates the movement alpha value. */
	UFUNCTION()
	void UpdateMovementAlpha(const float DeltaTime);

	/** Calculates the flashlight focus rotation.
	 *	@Return The target rotation for the flashlight to focus on whatever surface the player is looking at.
	 */
	UFUNCTION()
	FRotator GetFlashlightFocusRotation() const;

	/** Calculates the flashlight sway rotation.
	 *	@Return A rotator that can be added to the world rotation of the flashlight to introduce flashlight sway.
	 */
	UFUNCTION()
	FRotator GetFlashlightSwayRotation() const;

	/** Returns the flashlight socket rotation with an offset depending on the movement type of the PlayerCharacter.
	 *	@Socket The socket to get the rotation from.
	 *	@MovementType The current ground movement type of the player.
	 *	@Return The rotation of the socket with an offset depending on the ground movement type.
	 */
	UFUNCTION()
	FRotator GetSocketRotationWithOffset(const FName Socket, const EPlayerGroundMovementType MovementType) const;
};
