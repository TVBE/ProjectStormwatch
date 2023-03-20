// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerInteractionComponent.generated.h"

class UCameraComponent;
struct FCollisionQueryParams;

/** PlayerCharacter component that checks for interactable objects in front of the player character. */
UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = (PlayerCharacter), Meta = (BlueprintSpawnableComponent))
class FROSTBITE_API UPlayerInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	/** The camera component of the Player Character. */
	UPROPERTY(BlueprintReadOnly, Category = "PlayerInteractionComponent", Meta = (DisplayName = "Camera", AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

	/** The length of the initial line trace. */
	UPROPERTY(EditDefaultsOnly, Category = "PlayerInteractionComponent", Meta = (DisplayName = "Camera Trace Length", ClampMax = "500", UIMax = "500"))
	uint16 CameraTraceLength {300};

	/** The radius of the interactable object multi sphere trace. */
	UPROPERTY(EditDefaultsOnly, Category = "PlayerInteractionComponent", Meta = (DisplayName = "Object Trace Radius", ClampMax = "500", UIMax = "500"))
	uint16 ObjectTraceRadius {50};

	/** The location of the camera. */
	UPROPERTY()
	FVector CameraLocation;
	
	/** The hit result for the initial visibility line trace collision query performed from the camera. */
	UPROPERTY()
	FHitResult CameraTraceHitResult {FHitResult()};

	/** The hit result for the occlusion line trace collision query performed from the camera. */
	UPROPERTY()
	FHitResult OcclusionTraceHitResult {FHitResult()};

	/** The array of hit results for the interactable object multi sphere trace. */
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	TArray<FHitResult> ObjectTraceHitResults;

	/** The actor that currently can be interacted with. Will be a nullptr if no object can be interacted with at the moment. */
	UPROPERTY(BlueprintReadOnly, Category = "PlayerInteractionComponent", Meta = (DisplayName = "Current Interactable Actor", AllowPrivateAccess = "true"))
	AActor* CurrentInteractableActor;

	/** The collision query data for the camera trace. */
	FCollisionQueryParams CameraTraceQueryParams;

	/** The collision query data for the object multi sphere trace. */
	FCollisionObjectQueryParams ObjectTraceQueryParams;

	/** The timer for the update function. */
	UPROPERTY()
	FTimerHandle UpdateTimerHandle;

	/** When true, we will draw debug visualisation to the screen for every collision query. */
	UPROPERTY(EditAnywhere, Category = "PlayerInteractionComponent|Debugging", Meta = (DisplayName = "Enable Debug Visualisation"))
	bool IsDebugVisEnabled {false};
	
public:	
	UPlayerInteractionComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void OnRegister() override;
	virtual void OnUnregister() override;
	virtual void BeginPlay() override;
	
	/** Checks if any interactable objects are in front of the player. */
	UFUNCTION(BlueprintCallable, Category = "PlayerInteractionComponent", Meta = (DisplayName = "Check For Interactable Objects", BlueprintProtected))
	AActor* CheckForInteractableActor();

private:
	UFUNCTION()
	void PerformTraceFromCamera(FHitResult& HitResult);

	UFUNCTION()
	void PerformInteractableObjectTrace(TArray<FHitResult>& Array, const FHitResult& HitResult);

	UFUNCTION()
	static AActor* GetClosestObject(const TArray<FHitResult>& Array, const FHitResult& HitResult);

	UFUNCTION()
	bool IsActorOccluded(const AActor* Actor);
	
};
