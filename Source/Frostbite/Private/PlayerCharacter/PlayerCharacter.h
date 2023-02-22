// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class UPlayerCharacterConfiguration;
class UPlayerStateConfiguration;
class UPlayerCameraConfiguration;
class UPlayerFlashlightConfiguration;
class APlayerCharacterController;
class UCameraComponent;
class USpotLightComponent;
class USpringArmComponent;
class UPlayerAudioController;
class UPlayerVfxController;
class UPlayerCameraController;
class UPlayerFlashlightController;
class UPlayerCharacterMovementComponent;
class UNiagaraComponent;
enum class EFoot : uint8;
enum class EPlayerLandingType : uint8;
struct FFootstepData;
struct FTimerHandle;

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = (PlayerCharacter))
class APlayerCharacter : public ACharacter
{
	GENERATED_BODY()
	
public:
	/** If true, the character is currently jumping. We assume the character is jumping if the character has not left the ground yet, but the jump action is triggered. */
	UPROPERTY(BlueprintReadOnly, Category = "PlayerCharacter|Locomotion", Meta = (DisplayName = "Is Jumping"))
	bool IsJumping;

private:
	// CONFIGURATION
	/** The character configuration data asset. */
	UPROPERTY(BlueprintGetter = GetCharacterConfiguration, EditAnywhere, Category = "PlayerCharacter|Configuration", Meta = (DisplayName = "Character Configuration", DisplayPriority = "0"))
	UPlayerCharacterConfiguration* CharacterConfiguration;

	/** The playerstate configuration data asset. */
	UPROPERTY(BlueprintGetter = GetStateConfiguration, EditAnywhere, Category = "PlayerCharacter|Configuration", Meta = (DisplayName = "State Configuration", DisplayPriority = "1"))
	UPlayerStateConfiguration* StateConfiguration;
	
	/** The camera configuration data asset */
	UPROPERTY(BlueprintGetter = GetCameraConfiguration, EditAnywhere, Category = "PlayerCharacter|Configuration", Meta = (DisplayName = "Camera Configuration", DisplayPriority = "2"))
	UPlayerCameraConfiguration* CameraConfiguration;

	/** The flashlight configuration data asset*/
	UPROPERTY(BlueprintGetter = GetFlashlightConfiguration, EditAnywhere, Category = "PlayerCharacter|Configuration", Meta = (DisplayName = "Flashlight Configuration", DisplayPriority = "3"))
	UPlayerFlashlightConfiguration* FlashlightConfiguration;

	
	// COMPONENTS
	/** The camera for the player. */
	UPROPERTY(BlueprintGetter = GetCamera, EditAnywhere, Category = "PlayerCharacter|Camera", Meta = (DisplayName = "Camera"))
	UCameraComponent* Camera;

	/** The flashlight for the player. */
	UPROPERTY(BlueprintGetter = GetFlashlight, Category = "PlayerCharacter|Flashlight", Meta = (DisplayName = "Flashlight"))
	USpotLightComponent* Flashlight;

	/** The SpringArmComponent the flashlight is attached to. */
	UPROPERTY(BlueprintGetter = GetFlashlightSpringArm, Category = "PlayerCharacter|Flashlight", Meta = (DisplayName = "Flashlight Arm"))
	USpringArmComponent* FlashlightSpringArm;

	/** The PlayerAudioController that handles player audio. */
	UPROPERTY(BlueprintGetter = GetAudioController, Category = "PlayerCharacter|Components", Meta = (DisplayName = "Audio Controller"))
	UPlayerAudioController* AudioController;

	/** The PlayerVfxController that handles player VFX. */
	UPROPERTY(BlueprintGetter = GetVfxController, Category = "PlayerCharacter|Components", Meta = (DisplayName = "VFX Controller"))
	UPlayerVfxController* VfxController;

	/** The CameraController that handles first person camera behavior. */
	UPROPERTY(BlueprintGetter = GetCameraController, Category = "PlayerCharacter|Components", Meta = (DisplayName = "Camera Controller"))
	UPlayerCameraController* CameraController;

	/** The FlashlightController that handles the flashlight behavior. */
	UPROPERTY(BlueprintGetter = GetFlashlightController, Category = "PlayerCharacter|Components", Meta = (DisplayName = "Flashlight Controller"))
	UPlayerFlashlightController* FlashlightController;

	/** The PlayerCharacterMovementComponent that handles the PlayerCharacter's movement. */
	UPROPERTY(BlueprintGetter = GetPlayerCharacterMovement, EditAnywhere, Category = "PlayerCharacter|Components", Meta = (DisplayName = "Player Character Movement Component"))
	UPlayerCharacterMovementComponent* PlayerCharacterMovement;

	/** The AudioComponent for general player audio. */
	UPROPERTY(BlueprintGetter = GetBodyAudioComponent, VisibleAnywhere, Category = "PlayerCharacter|Components", Meta = (DisplayName = "Body Audio Component"))
	UAudioComponent* BodyAudioComponent;

	/** The AudioComponent for the player's left foot. */
	UPROPERTY(BlueprintGetter = GetLeftFootAudioComponent, VisibleAnywhere, Category = "PlayerCharacter|Components", Meta = (DisplayName = "Left Foot Audio Component"))
	UAudioComponent* LeftFootAudioComponent;

	/** The AudioComponent for the player's right foot.*/
	UPROPERTY(BlueprintGetter = GetRightFootAudioComponent, VisibleAnywhere, Category = "PlayerCharacter|Components", Meta = (DisplayName = "Right Foot Audio Component"))
	UAudioComponent* RightFootAudioComponent;

	/** The particle emitter for the player's left foot. */
	UPROPERTY(BlueprintGetter = GetLeftFootParticleEmitter, Category = "PlayerCharacter|Components", Meta = (DisplayName = "Left Foot Particle Emitter"))
	UNiagaraComponent* LeftFootParticleEmitter;
	
	/** The particle emitter for the player's right foot. */
	UPROPERTY(BlueprintGetter = GetRightFootParticleEmitter, Category = "PlayerCharacter|Components", Meta = (DisplayName = "Right Foot Particle Emitter"))
	UNiagaraComponent* RightFootParticleEmitter;

	/** The PlayerCharacterController that is currently controlling this PlayerCharacter. */
	UPROPERTY(BlueprintGetter = GetPlayerCharacterController, Category = "PlayerCharacter", Meta = (DisplayName = "Player Character Controller"))
	APlayerCharacterController* PlayerCharacterController;

	
	// VARIABLES
	/** If true, the character is currently turning in place. */
	UPROPERTY(BlueprintGetter = GetIsTurningInPlace, Category = "PlayerCharacter|Locomotion", Meta = (DisplayName = "Is Turning In Place"))
	bool IsTurningInPlace {false};

	/** The yaw delta value used for turn-in-place rotation. */
	UPROPERTY(BlueprintGetter = GetYawDelta, Category = "PlayerCharacter|Locomotion", Meta = (DisplayName = "Yaw Delta"))
	float YawDelta {0.f};

	// TIMERS
	/** The timer handle for the hard and heavy landing stun duration. */
	UPROPERTY()
	FTimerHandle FallStunTimer;

public:
	// Sets default values for this character's properties
	APlayerCharacter();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Called after the constructor but before BeginPlay().
	virtual void PostInitProperties() override;

	virtual void PostInitializeComponents() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Called after all default properties have been initialized. */
	virtual void OnConstruction(const FTransform& Transform) override;
	
	/** Called when the pawn is possessed by a controller. */
	virtual void PossessedBy(AController* NewController) override;

	/** Called when the pawn is ready to be destroyed or when the game ends. */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	/** Updates the character's rotation. */
	void UpdateRotation(float DeltaTime);

	/** Validates the configuration data assets. */
	void ValidateConfigurationAssets();

	/** Applies the configuration data assets to the character. */
	void ApplyConfigurationAssets();
	
	/** Specify whether the character is currently jumping. */
	UFUNCTION(BlueprintCallable, Category = "PlayerCharacter|Locomotion", Meta = (DisplayName = "Set IsJumping", CompactNodeTitle = "Is Jumping"))
	void SetIsJumping(bool Value);

private:
	/** Updates the character's yaw delta. */
	UFUNCTION()
	void UpdateYawDelta();
	
	/** Returns a scaled yaw delta value that is used for turn-in-place rotation.
	 *	@YawDelta The delta yaw rotation between the player's control rotation and the character's mesh rotation.
	 *	@DeltaTime The frame time in seconds.
	 *	@Factor A multiplier that affects the rotation speed.
	 *	@Clamp The maximum allowed YawDelta angle before the rotation speed should be clamped to prevent the camera from rotation too much in relation to the character's neck. //TODO: This doesn't work properly currently.
	 *	@Return A float value representing a rotator's yaw axis.
	 */
	static float CalculateTurnInPlaceRotation (const float YawDelta, const float DeltaTime, const float Factor, const float Clamp);

	/** Handles the landing callback from the player character movement component. */
	UFUNCTION()
	void HandleLanding(EPlayerLandingType Value);

	/** Handles the ending of a landing. */
	UFUNCTION()
	void HandleLandingEnd();

#if WITH_EDITOR
	/** Checks whether an object is properly initialized.
	 *	@Object The object to validate.
	 *	@Objectname The name of the object to be used in the log entry should the object not be properly initialized. 
	 */
	static void ValidateObject(const UObject* Object, const FString ObjectName);
#endif

public:
	/** Returns the Character configuration. */
	UFUNCTION(BlueprintGetter, Category = "PlayerCharacter|Configuration", Meta = (DisplayName = "Get Character Configuration"))
	FORCEINLINE UPlayerCharacterConfiguration* GetCharacterConfiguration() const {return CharacterConfiguration; }

	/** Returns the PlayerState configuration. */
	UFUNCTION(BlueprintGetter, Category = "PlayerCharacter|Configuration", Meta = (DisplayName = "Get State Configuration"))
	FORCEINLINE UPlayerStateConfiguration* GetStateConfiguration() const {return StateConfiguration; }

	/** Returns the Camera configuration. */
	UFUNCTION(BlueprintGetter, Category = "PlayerCharacter|Configuration", Meta = (DisplayName = "Get Camera Configuration"))
	FORCEINLINE UPlayerCameraConfiguration* GetCameraConfiguration() const {return CameraConfiguration; }

	/** Returns the Flashlight configuration. */
	UFUNCTION(BlueprintGetter, Category = "PlayerCharacter|Configuration", Meta = (DisplayName = "Get Flashlight Configuration"))
	FORCEINLINE UPlayerFlashlightConfiguration* GetFlashlightConfiguration() const {return FlashlightConfiguration; }
	
	/** Returns the PlayerCharacterController that is controlling this PlayerCharacter. */
	UFUNCTION(BlueprintGetter, Category = "PlayerCharacter|Locomotion", Meta = (DisplayName = "PlayerCharacterController"))
	FORCEINLINE APlayerCharacterController* GetPlayerCharacterController() const {return PlayerCharacterController; }
	
	/** Returns the camera component. */
	UFUNCTION(BlueprintGetter, Category = "PlayerCharacter|Components", Meta = (DisplayName = "Camera"))
	FORCEINLINE UCameraComponent* GetCamera() const {return Camera; }

	/** Returns the flashlight component. */
	UFUNCTION(BlueprintGetter, Category = "PlayerCharacter|Components", Meta = (DisplayName = "Flashlight"))
	FORCEINLINE USpotLightComponent* GetFlashlight() const {return Flashlight; }

	/** Returns the flashlight SpringArmComponent. */
	UFUNCTION(BlueprintGetter, Category = "PlayerCharacter|Components", Meta = (DisplayName = "Flashlight Spring Arm"))
	FORCEINLINE USpringArmComponent* GetFlashlightSpringArm() const {return FlashlightSpringArm; }

	/** Returns the Player Camera Controller. */
	UFUNCTION(BlueprintGetter, Category = "PlayerCharacter|Components", Meta = (DisplayName = "Camera Controller"))
	FORCEINLINE UPlayerCameraController* GetCameraController() const {return CameraController; }

	/** Returns the Player Flashlight Controller. */
	UFUNCTION(BlueprintGetter, Category = "PlayerCharacter|Components", Meta = (DisplayName = "Flashlight Controller"))
	FORCEINLINE UPlayerFlashlightController* GetFlashlightController() const {return FlashlightController; }
	
	/** Returns the Player Audio Controller. */
	UFUNCTION(BlueprintGetter, Category = "PlayerCharacter|Components", Meta = (DisplayName = "Player Audio Controller"))
	FORCEINLINE UPlayerAudioController* GetAudioController() const {return AudioController; }

	/** Returns the Player VFX Controller*/
	UFUNCTION(BlueprintGetter, Category = "PlayerCharacter|Components", Meta = (DisplayName = "Player VFX Controller"))
	FORCEINLINE UPlayerVfxController* GetVfxController() const {return VfxController; }

	/** Returns the PlayerCharacterMovementComponent. */
	UFUNCTION(BlueprintGetter, Category = "PlayerCharacter|Components", Meta = (DisplayName = "Player Character Movement Component"))
	FORCEINLINE UPlayerCharacterMovementComponent* GetPlayerCharacterMovement() const {return PlayerCharacterMovement; }

	/** Returns the body AudioComponent. */
	UFUNCTION(BlueprintGetter, Category = "PlayerCharacter|Components", Meta = (DisplayName = "Body Audio Component"))
	FORCEINLINE UAudioComponent* GetBodyAudioComponent() const {return BodyAudioComponent; }

	/** Returns the left foot AudioComponent. */
	UFUNCTION(BlueprintGetter, Category = "PlayerCharacter|Components", Meta = (DisplayName = "Left Foot Audio Component"))
	FORCEINLINE UAudioComponent* GetLeftFootAudioComponent() const {return LeftFootAudioComponent; }

	/** Returns the right foot AudioComponent. */
	UFUNCTION(BlueprintGetter, Category = "PlayerCharacter|Components", Meta = (DisplayName = "Right Foot Audio Component"))
	FORCEINLINE UAudioComponent* GetRightFootAudioComponent() const {return RightFootAudioComponent; }

	/** Returns the left foot ParticleSystem. */
	UFUNCTION(BlueprintGetter, Category = "PlayerCharacter|Components", Meta = (DisplayName = "Left Foot Particle Emitter"))
	FORCEINLINE UNiagaraComponent* GetLeftFootParticleEmitter() const {return LeftFootParticleEmitter; }

	/** Returns the right foot ParticleSystem. */
	UFUNCTION(BlueprintGetter, Category = "PlayerCharacter|Components", Meta = (DisplayName = "Right Foot Particle Emitter"))
	FORCEINLINE UNiagaraComponent* GetRightFootParticleEmitter() const {return RightFootParticleEmitter; }

	/** Returns if the character is currently turning in place. */
	UFUNCTION(BlueprintGetter, Category = "PlayerCharacter|Locomotion", Meta = (DisplayName = "Is Turning In Place"))
	FORCEINLINE bool GetIsTurningInPlace() const {return IsTurningInPlace; }

	/** Returns the character yaw delta between the facing direction of the character and the camera. */
	UFUNCTION(BlueprintGetter, Category = "PlayerCharacter|Locomotion", Meta = (DisplayName = "Yaw Delta"))
	FORCEINLINE float GetYawDelta() const {return YawDelta; }
	
};




