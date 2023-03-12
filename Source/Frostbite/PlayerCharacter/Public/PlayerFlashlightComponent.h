// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerFlashlightComponent.generated.h"

class UPlayerCharacterMovementComponent;
class UCameraComponent;
class APlayerCharacter;
class UPlayerFlashlightConfiguration;
enum class EPlayerGroundMovementType : uint8;

/** UPlayerFlashlightController is an Actor Component responsible for controlling the player's flashlight. 
 *	This class provides a simple and convenient way for designers to customize the player's flashlight behavior.
 *	@Brief ActorComponent for controlling the player's flashlight.
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup = (PlayerCharacter), Meta = (BlueprintSpawnableComponent)) // TODO: Make SuperClass for LightSource items.
class UPlayerFlashlightComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	// COMPONENTS
	/** The flashlight for the player. */
	UPROPERTY(BlueprintGetter = GetFlashlight, Category = "PlayerCharacter|Flashlight", Meta = (DisplayName = "Flashlight"))
	USpotLightComponent* Flashlight;

	/** The SpringArmComponent the flashlight is attached to. */
	UPROPERTY(BlueprintGetter = GetFlashlightSpringArm, Category = "PlayerCharacter|Flashlight", Meta = (DisplayName = "Flashlight Arm"))
	USpringArmComponent* FlashlightSpringArm;

	// CONFIGURATION
	/** The configuration asset to use for this component. */
	UPROPERTY(EditAnywhere, Category = "Configuration", Meta = (DisplayName = "Configuration"))
	TSoftObjectPtr<UPlayerFlashlightConfiguration> ConfigurationAsset;

	/** Pointer to the configuration asset for this component. */
	UPROPERTY()
	UPlayerFlashlightConfiguration* Configuration;

	// VARIABLES
	/** Pointer to the camera of the owner. */
	UPROPERTY(BlueprintReadOnly, Category = "PlayerFlashlightController", Meta = (DisplayName = "Mesh", AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh;

	/** Pointer to the mesh of the owner. */
	UPROPERTY(BlueprintReadOnly, Category = "PlayerFlashlightController", Meta = (DisplayName = "Camera", AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

	/** Pointer to the player character movement component of the owner.*/
	UPROPERTY(BlueprintReadOnly, Category = "PlayerFlashlightController", Meta = (DisplayName = "Player Character Movement Component", AllowPrivateAccess = "true"))
	UPlayerCharacterMovementComponent* Movement;
	
	/** Alpha value for blending the flashlight rotation based on movement. */
	UPROPERTY(BlueprintReadOnly, Category = "PlayerFlashlightController", Meta = (DisplayName = "Movement Alpha", AllowPrivateAccess = "true"))
	float MovementAlpha {0.f};
	
public:	
	// Sets default values for this component's properties
	UPlayerFlashlightComponent();
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Enables or disables the flashlight. */
	UFUNCTION(BlueprintCallable, Category = "PlayerFlashlightController", Meta = (DisplayName = "Set Flashlight Enabled"))
	void SetFlashlightEnabled(const bool Value);

	/** Returns whether the flashlight is enabled or not. */
	UFUNCTION(BlueprintPure, Category = "PlayerFlashlightController", Meta = (DisplayName = "Is Flashlight Enabled"))
	bool IsFlashlightEnabled() const;

	/** Updates the movement alpha value. */
	void UpdateMovementAlpha(const float DeltaTime);

	/** Calculates the flashlight focus rotation.
	 *	@Return The target rotation for the flashlight to focus on whatever surface the player is looking at.
	 */
	FRotator GetFlashlightFocusRotation() const;

	/** Calculates the flashlight sway rotation.
	 *	@Return A rotator that can be added to the world rotation of the flashlight to introduce flashlight sway.
	 */
	FRotator GetFlashlightSwayRotation() const;

	/** Returns the flashlight socket rotation with an offset depending on the movement type of the PlayerCharacter.
	 *	@Socket The socket to get the rotation from.
	 *	@MovementType The current ground movement type of the player.
	 *	@Return The rotation of the socket with an offset depending on the ground movement type.
	 */
	FRotator GetSocketRotationWithOffset(const FName Socket, const EPlayerGroundMovementType MovementType) const;

protected:
	virtual void OnRegister() override;
	virtual void OnUnregister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void CleanupComponent();

public:
	/** Returns the flashlight component. */
	UFUNCTION(BlueprintGetter, Category = "PlayerCharacter|Components", Meta = (DisplayName = "Flashlight"))
	FORCEINLINE USpotLightComponent* GetFlashlight() const {return Flashlight; }

	/** Returns the flashlight SpringArmComponent. */
	UFUNCTION(BlueprintGetter, Category = "PlayerCharacter|Components", Meta = (DisplayName = "Flashlight Spring Arm"))
	FORCEINLINE USpringArmComponent* GetFlashlightSpringArm() const {return FlashlightSpringArm; }

	/** Returns the Flashlight configuration. */
	UFUNCTION(BlueprintGetter, Category = "PlayerCharacter|Configuration", Meta = (DisplayName = "Get Flashlight Configuration"))
	FORCEINLINE UPlayerFlashlightConfiguration* GetFlashlightConfiguration() const {return Configuration; }
};
