// Copyright (c) 2022-present Barrelhouse. All rights reserved.
// Written by Tim Verberne.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacterController.h"
#include "PlayerMovementComponent.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

UENUM(BlueprintType)
enum class ECrouchToggleMode : uint8
{
	PressAndHold	UMETA(DisplayName = "Press And Hold"),
	Toggle			UMETA(DisplayName = "Toggle")
};

UCLASS(Abstract, Blueprintable, BlueprintType, NotPlaceable, ClassGroup = "PlayerCharacter")
class STORMWATCH_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

	virtual void Tick(float DeltaTime) override;

	virtual void Jump() override;
	bool CanPerformJump() const;

	virtual void Crouch(bool bClientSimulation) override;
	virtual void UnCrouch(bool bClientSimulation) override;
	bool CanCrouch() const override;
	bool CanStandUp() const;
	float GetClearanceAbovePawn() const;

	void StartSprinting();
	void StopSprinting();

	bool GetIsTurningInPlace() const { return IsTurningInPlace; }
	float GetYawDelta() const { return YawDelta; }

	bool IsSprinting() const
	{
		if (PlayerMovement)
		{
			return PlayerMovement->GetIsSprinting();
		}
		return false;
	}

	UFUNCTION(BlueprintGetter)
	class UCameraComponent* GetCamera() const
	{
		check(Camera);
		return Camera;
	}

	UFUNCTION(BlueprintGetter)
	class UPlayerCameraController* GetCameraController() const
	{
		check(CameraController);
		return CameraController;
	}

	UFUNCTION(BlueprintGetter)
	class UPlayerMovementComponent* GetPlayerMovement() const
	{
		check(PlayerMovement);
		return PlayerMovement;
	}

	UFUNCTION(BlueprintGetter)
	UPlayerInteractionComponent* GetInteractionComponent() const
	{
		check(InteractionComponent);
		return InteractionComponent;
	}

	UFUNCTION(BlueprintGetter)
	class UPlayerUseComponent* GetUseComponent() const
	{
		check(UseComponent);
		return UseComponent;
	}

	UFUNCTION(BlueprintGetter)
	class UPlayerGrabComponent* GetGrabComponent() const
	{
		check(GrabComponent);
		return GrabComponent;
	}

	UFUNCTION(BlueprintGetter)
	class UPlayerDragComponent* GetDragComponent() const
	{
		check(DragComponent);
		return DragComponent;
	}

	UFUNCTION(BlueprintGetter)
	class UPlayerInventoryComponent* GetInventoryComponent() const
	{
		check(InventoryComponent);
		return InventoryComponent;
	}

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void PossessedBy(AController* NewController) override;

	void UpdateRotation(const float& DeltaTime);
	void UpdateMovementSpeed();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	FPlayerCharacterSettings Settings = FPlayerCharacterSettings();

	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetCamera, Category = "Components")
	UCameraComponent* Camera;

	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetCameraController, Category = "Components")
	UPlayerCameraController* CameraController;

	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetPlayerMovement, Category = "Components",
			  Meta = (DisplayName = "Player Movement Component"))
	UPlayerMovementComponent* PlayerMovement;

	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetInteractionComponent, Category = "Components")
	UPlayerInteractionComponent* InteractionComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetUseComponent, Category = "Components")
	UPlayerUseComponent* UseComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetGrabComponent, Category = "Components")
	UPlayerGrabComponent* GrabComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetDragComponent, Category = "Components")
	UPlayerDragComponent* DragComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetInventoryComponent, Category = "Components")
	UPlayerInventoryComponent* InventoryComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class UPlayerBodyCollisionComponent* BodyCollision;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class UPlayerFootCollisionComponent* LeftFootCollision;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UPlayerFootCollisionComponent* RightFootCollision;

private:
	float YawDelta = 0.f;
	bool IsTurningInPlace = false;

	UPROPERTY()
	FTimerHandle FallStunTimer;

private:
	UFUNCTION()
	void UpdateYawDelta();

	static float CalculateTurnInPlaceRotation(const float YawDelta, const float DeltaTime, const float Factor, const float Clamp);

	UFUNCTION()
	void HandleLandingStart(EPlayerLandingType Value);
	UFUNCTION()
	void HandleLandingEnd();
};




