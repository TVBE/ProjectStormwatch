// Copyright 2023 Barrelhouse

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "PlayerPhysicsGrabComponent.generated.h"

/**
 * 
 */

class UCameraComponent;
class APlayerCharacter;

UCLASS(Blueprintable, ClassGroup=PlayerCharacter, meta=(BlueprintSpawnableComponent))
class UPlayerPhysicsGrabComponent : public UPhysicsHandleComponent
{
	GENERATED_BODY()
	
public:

	// CONFIGURATION
	/** The configuration asset to use for this component. */
	UPROPERTY(EditAnywhere, Category = "Configuration", Meta = (DisplayName = "Configuration"))
	TSoftObjectPtr<UPlayerPhysicsGrabConfiguration> ConfigurationAsset;

	/** Pointer to the configuration asset for this component. */
	UPROPERTY()
	UPlayerPhysicsGrabConfiguration* Configuration;

	//The camera component of the player
	UPROPERTY(EditAnywhere, Category = "PlayerCameraReference", Meta = (DisplayName = "PlayerCamera"))
	UCameraComponent* Camera;
	
	// The object that will be passed to the physics handle.
	UFUNCTION(BlueprintCallable, Category="Player Physics Grab")
	void GrabObject(AActor* ObjectToGrab);

	// Called when you release the object or place the object
	UFUNCTION(BlueprintCallable, Category="Player Physics Grab")
	void ReleaseObject();
	
	// Will be updated when a component is being grabbed.
	UFUNCTION(BlueprintCallable, Category="Player Physics Grab")
	void UpdateDesiredLocation(float ZoomAxisValue);

protected:
	virtual void OnRegister() override;
	//virtual void OnUnregister() override;
	//virtual void BeginPlay() override;
	//virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
private:

	
	UPROPERTY()
	float CurrentZoomLevel;
};

UCLASS(BlueprintType, ClassGroup = (PlayerCharacter))
class FROSTBITE_API UPlayerPhysicsGrabConfiguration : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly,Category="Player Physics Grab")
	FVector RelativeHoldingHandLocation;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Player Physics Grab")
	float MinZoomLevel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Player Physics Grab")
	float MaxZoomLevel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Player Physics Grab")
	float ZoomSpeed;

	/** Constructor with default values. */
	UPlayerPhysicsGrabConfiguration()
	{
	}
	void ApplyToPlayerPhysicsGrabComponent(const UPlayerPhysicsGrabComponent* Component);
};