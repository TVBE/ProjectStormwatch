// Copyright (c) 2022-present Barrelhouse. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BHPlayerCharacterController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerMovementInputLockDelegate, bool, bValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerRotationInputLockDelegate, bool, bValue);

/** The PlayerController for the PlayerCharacter. This class is responsible for handling all user input to the player Pawn. */
UCLASS(Blueprintable, BlueprintType, ClassGroup = "PlayerCharacter")
class STORMWATCH_API ABHPlayerCharacterController : public APlayerController
{
	GENERATED_BODY()

public:
	ABHPlayerCharacterController();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void OnPossess(APawn* InPawn) override;

	virtual void ProcessPlayerInput(float DeltaTime, bool bGamePaused) override;
	virtual void SetupInputComponent() override;

	virtual void Tick(float DeltaSeconds) override;

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

	/**
	 *  Adds or removes a movement input lock for the player controller. The player controller can only process movement input if there are no locks present.
	 *
	 *	@param Value		Whether a lock should be added or removed.
	 *	@return				If the player can now process movement input or not. This will only be the case if there are zero locks present.
	 */
	UFUNCTION(BlueprintCallable, Category = "Input")
	bool SetPlayerMovementInputLock(bool bValue);

	/**
	 *  Adds or removes a movement input lock for the player controller. The player controller can only process movement input if there are no locks present.
	 *
	 *  @param Value		Whether a lock should be added or removed.
	 *	@return				If the player can now process movement input or not. This will only be the case if there are zero locks present.
	 */
	UFUNCTION(BlueprintCallable, Category = "Input")
	bool SetPlayerRotationInputLock(bool bValue);

	/**
	 *  Fades in the screen for the player from black by a specified amount.
	 *
	 *	@param Duration		The fade-in duration.
	 */
	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Fade From Black"))
	void FadePlayerCameraFromBlack(float Duration);

	/** Returns whether the player controller can process movement input. */
	UFUNCTION(BlueprintGetter)
	bool GetCanProcessMovementInput() const { return bProcessMovementInput; }

	/** Returns whether the player controller can process rotation input. */
	UFUNCTION(BlueprintGetter)
	bool GetCanProcessRotationInput() const { return bProcessRotationInput; }

	/** Returns the player control rotation. */
	UFUNCTION(BlueprintGetter)
	FRotator GetPlayerControlRotation() const { return PlayerControlRotation; }

	/** Delegate for when the player controller should start or stop processing player movement input. */
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FPlayerMovementInputLockDelegate OnMovementInputLockChanged;

	/** Delegate for when the player controller should start or stop processing player rotation input. */
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FPlayerRotationInputLockDelegate OnRotationInputLockChanged;

protected:
	/** Performs a collision query in front of the camera and returns the hit result. */
	UFUNCTION(BlueprintPure, Category = "Controller", Meta = (DisplayName = "Get Camera Look At Query"))
	FHitResult GetCameraLookAtQuery() const;

	/** Pointer to the controlled pawn as a PlayerCharacter instance.*/
	UPROPERTY(BlueprintReadOnly)
	class ABHPlayerCharacter* PlayerCharacter = nullptr;

	/** If true, the player is currently pressing the sprint button. */
	UPROPERTY(BlueprintReadOnly, Category = "Input")
	bool bSprintPending = false;

	/** If true, the player is currently pressing the crouch button. */
	UPROPERTY(BlueprintReadOnly, Category = "Input")
	bool bCrouchPending = false;

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

	/** Tries to find an InteractionComponent in the player character. */
	UFUNCTION()
	class UBHPlayerInteractionComponent* SearchForPlayerInteractionComponent();

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

	/** Handles the callback for when the player has pressed the Jump button */
	UFUNCTION()
	void HandleJumpActionPressed();

	/** Handles the callback for when the player has pressed the Sprint button. */
	UFUNCTION()
	void HandleSprintActionPressed();

	/** Handles the callback for when the player has released the Sprint button. */
	UFUNCTION()
	void HandleSprintActionReleased();

	/** Handles the callback for when the player has pressed the Crouch button. */
	UFUNCTION()
	void HandleCrouchActionPressed();

	/** Handles the callback for when the player has released the Crouch button. */
	UFUNCTION()
	void HandleCrouchActionReleased();

	/** Handles the callback for when the player has pressed the ToggleFlashlight button. */
	UFUNCTION()
	void HandleFlashlightActionPressed();

	/** Handles the callback for when the player has pressed the PrimaryAction button. */
	UFUNCTION()
	void HandlePrimaryActionPressed();

	/** Handles the callback for when the player has released the PrimaryAction button. */
	UFUNCTION()
	void HandlePrimaryActionReleased();

	/** Handles the callback for when the player has pressed the SecondaryAction button. */
	UFUNCTION()
	void HandleSecondaryActionPressed();

	/** Handles the callback for when the player has released the SecondaryAction button. */
	UFUNCTION()
	void HandleSecondaryActionReleased();

	/** Handles the callback for when the player has pressed the RotateObject button. */
	UFUNCTION()
	void HandleTertiaryActionPressed();

	/** Handles the callback for when the player has released the RotateObject button. */
	UFUNCTION()
	void HandleTertiaryActionReleased();

	/** Handles the callback for when the player has pressed the InventoryAction button. */
	UFUNCTION()
	void HandleInventoryActionPressed();

	/** Handles the callback for when the player has released the InventoryAction button. */
	UFUNCTION()
	void HandleInventoryActionReleased();

	/** Pointer to the interaction component of the player character. */
	UPROPERTY()
	UBHPlayerInteractionComponent* InteractionComponent;

	/** Integer value that is incremented or decremented when another object calls SetPlayerMovementInputLock.
	 *	If the value is zero, CanProcessMovementInput will be set to true for the player controller.*/
	uint8 MovementInputLockCount = 1;

	/** Integer value that is incremented or decremented when another object calls SetPlayerRotationInputLock.
	 *	If the value is zero, CanProcessRotationInput will be set to true for the player controller.*/
	uint8 RotationInputLockCount = 1;

	/** When true, the player can receive user input for movement. */
	UPROPERTY(BlueprintGetter = GetCanProcessMovementInput)
	bool bProcessMovementInput = false;

	/** When true, the player can receive user input for camera rotation. */
	UPROPERTY(BlueprintGetter = GetCanProcessRotationInput)
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
