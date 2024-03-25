// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "Stormwatch/Actors/Public/BHCharacter.h"
#include "BHPlayerCharacter.generated.h"

UCLASS(Abstract, NotPlaceable, Blueprintable, BlueprintType, ClassGroup = "BHPlayerCharacter")
class STORMWATCH_API ABHPlayerCharacter : public ABHCharacter
{
	GENERATED_BODY()

public:
	ABHPlayerCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaTime) override;
	
	bool GetIsTurningInPlace() const { return bTurningInPlace; }
	float GetYawDelta() const { return YawDelta; }
	
	UFUNCTION(BlueprintGetter)
	class UBHPlayerCameraComponent* GetCamera() const;

	UFUNCTION(BlueprintGetter)
	class UBHPlayerCameraSocketComponent* GetCameraSocketComponent() const;

	UFUNCTION(BlueprintGetter)
	class UBHPlayerSkeletalMeshComponent* GetPlayerMesh() const;

	UFUNCTION(BlueprintGetter)
	class UBHCharacterMovementComponent* GetPlayerMovementComponent() const;

	UFUNCTION(BlueprintGetter)
	class UBHPlayerInteractionComponent* GetInteractionComponent() const;

	UFUNCTION(BlueprintGetter)
	class UBHPlayerUseComponent* GetUseComponent() const;

	UFUNCTION(BlueprintGetter)
	class UBHPlayerGrabComponent* GetGrabComponent() const;

	UFUNCTION(BlueprintGetter)
	class UBHPlayerDragComponent* GetDragComponent() const;

	UFUNCTION(BlueprintGetter)
	class UBHPlayerInventoryComponent* GetInventoryComponent() const;

	UFUNCTION(BlueprintPure, Category = "ABHPlayerCharacter")
	bool IsInteracting() const;

protected:
	virtual void PostLoad() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

	void UpdateRotation(const float& DeltaTime);
	
	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetCamera, Category = "Components")
	UBHPlayerCameraComponent* Camera;

	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetCameraSocketComponent, Category = "Components")
	UBHPlayerCameraSocketComponent* CameraSocketComponent;

	UPROPERTY(BlueprintGetter = GetPlayerMesh, Category = "Components")
	UBHPlayerSkeletalMeshComponent* PlayerMesh;

	UPROPERTY(BlueprintGetter = GetPlayerMovementComponent, Category = "Components")
	UBHCharacterMovementComponent* PlayerMovementComponent;

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
	class UBHPlayerBodyComponent* BodyCollision;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class UBHPlayerFootComponent* LeftFootComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UBHPlayerFootComponent* RightFootComponent;

private:
	float YawDelta = 0.f;
	bool bTurningInPlace = false;

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




