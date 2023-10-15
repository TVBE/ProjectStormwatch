// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BHPlayerController.h"
#include "BHPlayerMovementComponent.h"
#include "GameFramework/Character.h"
#include "BHPlayerCharacter.generated.h"

UCLASS(Abstract, Blueprintable, BlueprintType, NotPlaceable, ClassGroup = "PlayerCharacter")
class STORMWATCH_API ABHPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABHPlayerCharacter();

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

	bool bIsSprinting() const
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
	class UBHPlayerCameraController* GetCameraController() const
	{
		check(CameraController);
		return CameraController;
	}

	UFUNCTION(BlueprintGetter)
	class UBHPlayerMovementComponent* GetPlayerMovement() const
	{
		check(PlayerMovement);
		return PlayerMovement;
	}

	UFUNCTION(BlueprintGetter)
	UBHPlayerInteractionComponent* GetInteractionComponent() const
	{
		check(InteractionComponent);
		return InteractionComponent;
	}

	UFUNCTION(BlueprintGetter)
	class UBHPlayerUseComponent* GetUseComponent() const
	{
		check(UseComponent);
		return UseComponent;
	}

	UFUNCTION(BlueprintGetter)
	class UBHPlayerGrabComponent* GetGrabComponent() const
	{
		check(GrabComponent);
		return GrabComponent;
	}

	UFUNCTION(BlueprintGetter)
	class UBHPlayerDragComponent* GetDragComponent() const
	{
		check(DragComponent);
		return DragComponent;
	}

	UFUNCTION(BlueprintGetter)
	class UBHPlayerInventoryComponent* GetInventoryComponent() const
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
	UBHPlayerCameraController* CameraController;

	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetPlayerMovement, Category = "Components",
			  Meta = (DisplayName = "Player Movement Component"))
	UBHPlayerMovementComponent* PlayerMovement;

	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetInteractionComponent, Category = "Components")
	UBHPlayerInteractionComponent* InteractionComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetUseComponent, Category = "Components")
	UBHPlayerUseComponent* UseComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetGrabComponent, Category = "Components")
	UBHPlayerGrabComponent* GrabComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetDragComponent, Category = "Components")
	UBHPlayerDragComponent* DragComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetInventoryComponent, Category = "Components")
	UBHPlayerInventoryComponent* InventoryComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class UBHPlayerBodyCollisionComponent* BodyCollision;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class UBHPlayerFootCollisionComponent* LeftFootCollision;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UBHPlayerFootCollisionComponent* RightFootCollision;

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
	void HandleLandingStart(EBHPlayerLandingType Value);
	UFUNCTION()
	void HandleLandingEnd();
};




