// Copyright 2023 Barrelhouse

#pragma once

#include "CoreMinimal.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "PlayerPhysicsGrabComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup=PlayerCharacter, meta=(BlueprintSpawnableComponent))
class UPlayerPhysicsGrabComponent : public UPhysicsHandleComponent
{
	GENERATED_BODY()
	
public:
	UPlayerPhysicsGrabComponent();

	UFUNCTION(BlueprintCallable, Category="Player Physics Grab")
	void GrabObject(AActor* ObjectToGrab);

	UFUNCTION(BlueprintCallable, Category="Player Physics Grab")
	void ReleaseObject();

	UFUNCTION(BlueprintCallable, Category="Player Physics Grab")
	void UpdateZoomLevel();

	UFUNCTION(BlueprintCallable, Category="Player Physics Grab")
	void UpdateDesiredLocation(float ZoomAxisValue);
	
protected:
	
	UPROPERTY(BlueprintReadOnly,Category="Player Physics Grab")
	FVector RelativeHoldingHandLocation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player Physics Grab")
	float MinZoomLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player Physics Grab")
	float MaxZoomLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player Physics Grab")
	float ZoomSpeed;

	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player Physics Grab")
	float MoveSpeed;

	UPROPERTY(BlueprintReadOnly, Category="Player Physics Grab")
	float CurrentZoomLevel;

	UPROPERTY(BlueprintReadOnly, Category="Player Physics Grab")
	FVector DesiredLocation;
};
