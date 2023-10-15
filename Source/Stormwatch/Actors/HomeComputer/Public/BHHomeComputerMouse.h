// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BHHomeComputerMouse.generated.h"

class UBHGrabComponent;
class UStaticMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMouseMovementDelegate, FVector2D, Vector);

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "Desktop")
class ABHHomeComputerMouse : public AActor
{
	GENERATED_BODY()

public:
	ABHHomeComputerMouse();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintAssignable, Category = "Delegates", Meta = (DisplayName = "On Mouse Movement"))
	FOnMouseMovementDelegate OnMouseMovement;
	
protected:
	/** The static mesh component for the mouse. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", Meta = (DisplayName = "Static Mesh"))
	UStaticMeshComponent* MouseMesh;

	/** The mesh interaction component for the mouse. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UBHGrabComponent* GrabComponent;

public:	


protected:
	virtual void PostInitProperties() override;
	virtual void BeginPlay() override;

	/** Handles the mesh waking up. */
	UFUNCTION()
	void HandleOnMeshWake(UPrimitiveComponent* WakingComponent, FName Name);

	/** Handles the mesh going to sleep. */
	UFUNCTION()
	void HandleOnMeshSleep(UPrimitiveComponent* SleepingComponent, FName Name);
};
