// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "GameFramework/Character.h"
#include "BHPlayerCharacter.generated.h"

UCLASS(Abstract, Blueprintable, BlueprintType, NotPlaceable, ClassGroup = "PlayerCharacter")
class STORMWATCH_API ABHPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABHPlayerCharacter(const FObjectInitializer& ObjectInitializer);

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

	bool bIsSprinting() const;
	
	UFUNCTION(BlueprintGetter)
	class UBHPlayerCameraComponent* GetCamera() const;

	UFUNCTION(BlueprintGetter)
	class UBHPlayerCameraController* GetCameraController() const;

	UFUNCTION(BlueprintGetter)
	class UBHPlayerSkeletalMeshComponent* GetPlayerMesh() const;

	UFUNCTION(BlueprintGetter)
	class UBHPlayerMovementComponent* GetPlayerMovementComponent() const;

	UFUNCTION(BlueprintGetter)
	UBHPlayerInteractionComponent* GetInteractionComponent() const;

	UFUNCTION(BlueprintGetter)
	class UBHPlayerUseComponent* GetUseComponent() const;

	UFUNCTION(BlueprintGetter)
	class UBHPlayerGrabComponent* GetGrabComponent() const;

	UFUNCTION(BlueprintGetter)
	class UBHPlayerDragComponent* GetDragComponent() const;

	UFUNCTION(BlueprintGetter)
	class UBHPlayerInventoryComponent* GetInventoryComponent() const;

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void PossessedBy(AController* NewController) override;

	void UpdateRotation(const float& DeltaTime);
	void UpdateMovementSpeed();
	
	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetCamera, Category = "Components")
	class UBHPlayerCameraComponent* Camera;

	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetCameraController, Category = "Components")
	UBHPlayerCameraController* CameraController;

	UPROPERTY(BlueprintGetter = GetPlayerMesh, Category = "Components")
	class UBHPlayerSkeletalMeshComponent* PlayerMesh;

	UPROPERTY(BlueprintGetter = GetPlayerMovementComponent, Category = "Components")
	UBHPlayerMovementComponent* PlayerMovementComponent;

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




