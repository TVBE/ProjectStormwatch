// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BHPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerMovementInputLockDelegate, bool, bValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerRotationInputLockDelegate, bool, bValue);

UCLASS(Blueprintable, BlueprintType, ClassGroup = "Barrelhouse")
class STORMWATCH_API ABHPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ABHPlayerController();
	
	virtual void ProcessPlayerInput(float DeltaTime, bool bGamePaused) override;
	virtual void SetupInputComponent() override;

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintGetter)
	class UBHPlayerInteractionComponent* GetInteractionComponent() const;

	UFUNCTION(BlueprintPure, Category = "ABHPlayerController")
	class ABHPlayerCharacter* GetControlledCharacter() const;

	/** Returns whether the PlayerController has any movement input or not. */
	UFUNCTION(BlueprintPure, Category = "Input", Meta = (DisplayName = "Has Any Movement Input"))
	bool GetHasMovementInput() const;

	/** Returns the current horizontal rotation input value from the PlayerController. */
	UFUNCTION(BlueprintPure, Category = "Input")
	float GetHorizontalRotationInput() const;

	/** Sets CanProcessMovementInput. This function can only be called by a PlayerSubsystem. */
	void SetCanProcessMovementInput(bool bValue);

	/** Sets CanProcessRotationInput. This function can only be called by a PlayerSubsystem. */
	void SetCanProcessRotationInput(bool bValue);
	
	UFUNCTION(BlueprintCallable, Category = "BHPlayerController")
	bool SetMovementInputLock(bool bValue);
	
	UFUNCTION(BlueprintCallable, Category = "BHPlayerController")
	bool SetRotationInputLock(bool bValue);
	
	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Fade From Black"))
	void FadePlayerCameraFromBlack(float Duration);

	UFUNCTION(BlueprintPure, Category = "BHPlayerController")
	bool CanProcessMovementInput() const;

	UFUNCTION(BlueprintGetter, Category = "BHPlayerController")
	bool CanProcessRotationInput() const;

	UFUNCTION(BlueprintGetter)
	FRotator GetPlayerControlRotation() const;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void OnPossess(APawn* InPawn) override;

private:
	/** Updates the player control rotation. */
	UFUNCTION()
	void UpdatePlayerControlRotation(const FRotator& Rotation, const float DeltaSeconds);

	/** Checks if the player character can currently sprint. */
	UFUNCTION()
	bool CanCharacterSprint() const;

	/** Checks if any player actions are currently pending and tries to complete them. */
	UFUNCTION()
	void UpdatePendingActions();

	/** Checks if the player can continue with any actions they are currently performing. */
	UFUNCTION()
	void UpdateCurrentActions();
	
	void CalculateRotationMultiplier(const FVector2D InputDirection);

	/** Adjusts the character's horizontal orientation using a gamepad or mouse. */
	UFUNCTION()
	void HandleHorizontalRotation(float Value);

	/** Adjusts the character's vertical orientation using a gamepad or mouse. */
	UFUNCTION()
	void HandleVerticalRotation(float Value);

	/** Tries to move the character forward or backward depending on the input. */
	UFUNCTION()
	void HandleLongitudinalMovementInput(float Value);

	/** Tries to move the character forward or backward depending on the input. */
	UFUNCTION()
	void HandleLateralMovementInput(float Value);

	/** Handles the callback for when the player tries to zoom in. */
	UFUNCTION()
	void HandleZoomDirectionInput(float Value);

	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetInteractionComponent, Category = "Components")
	UBHPlayerInteractionComponent* InteractionComponent;

	/** Integer value that is incremented or decremented when another object calls SetMovementInputLock.
	 *	If the value is zero, CanProcessMovementInput will be set to true for the player controller.*/
	uint8 MovementInputLockCount = 1;

	/** Integer value that is incremented or decremented when another object calls SetRotationInputLock.
	 *	If the value is zero, CanProcessRotationInput will be set to true for the player controller.*/
	uint8 RotationInputLockCount = 1;

	UPROPERTY()
	bool bProcessMovementInput = false;

	UPROPERTY()
	bool bProcessRotationInput = false;

	/** Smoothed control rotation. */
	UPROPERTY(BlueprintGetter = GetPlayerControlRotation)
	FRotator PlayerControlRotation;

	/** The interpolation speed of the player control rotation.*/
	UPROPERTY(EditAnywhere, Category = "Control Rotation", Meta = (DisplayName = "Interpolation Speed"))
	float ControlInterpolationSpeed = 10.0f;

	/** The rotation multiplier for the control rotation. */
	float InteractionRotationMultiplier = 1.0f;

	FVector2D InputRotation;
};
