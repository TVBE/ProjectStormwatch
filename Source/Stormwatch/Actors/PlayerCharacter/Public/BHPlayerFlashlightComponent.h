// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "BHPlayerCharacterComponent.h"
#include "BHPlayerFlashlightComponent.generated.h"

class UBHPlayerMovementComponent;
class UCameraComponent;
class ABHPlayerCharacter;
class UPlayerFlashlightConfiguration;
enum class EBHPlayerGroundMovementType : uint8;

UCLASS(Blueprintable, BlueprintType, ClassGroup = "PlayerCharacter", Meta = (BlueprintSpawnableComponent))
class STORMWATCH_API UBHPlayerFlashlightComponent : public UBHPlayerCharacterComponent
{
	GENERATED_BODY()

public:
	UBHPlayerFlashlightComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void SetFlashlightEnabled(bool bValue);

	UFUNCTION(BlueprintPure)
	bool IsFlashlightEnabled() const;

	void UpdateMovementAlpha(const float DeltaTime);

	FRotator GetFlashlightFocusRotation() const;
	FRotator GetFlashlightSwayRotation() const;

	FRotator GetSocketRotationWithOffset(const FName Socket, const EBHPlayerGroundMovementType MovementType) const;

	UFUNCTION(BlueprintGetter)
	USpotLightComponent* GetFlashlight() const { return Flashlight; }

	UFUNCTION(BlueprintGetter)
	USpringArmComponent* GetFlashlightSpringArm() const { return FlashlightSpringArm; }

	UFUNCTION(BlueprintGetter)
	UPlayerFlashlightConfiguration* GetFlashlightConfiguration() const { return Configuration; }

protected:
	virtual void OnRegister() override;
	virtual void OnUnregister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	/** The flashlight for the player. */
	UPROPERTY(BlueprintGetter = GetFlashlight)
	USpotLightComponent* Flashlight;

	/** The SpringArmComponent the flashlight is attached to. */
	UPROPERTY(BlueprintGetter = GetFlashlightSpringArm)
	USpringArmComponent* FlashlightSpringArm;

	/** The configuration asset to use for this component. */
	UPROPERTY(EditAnywhere, Category = "Configuration", Meta = (DisplayName = "Configuration"))
	TSoftObjectPtr<UPlayerFlashlightConfiguration> ConfigurationAsset;

	/** Pointer to the configuration asset for this component. */
	UPROPERTY()
	UPlayerFlashlightConfiguration* Configuration;

	/** Pointer to the camera of the owner. */
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh;

	/** Pointer to the mesh of the owner. */
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

	/** Pointer to the player character movement component of the owner.*/
	UPROPERTY(BlueprintReadOnly, Meta = (DisplayName = "Player Character Movement Component", AllowPrivateAccess = "true"))
	UBHPlayerMovementComponent* Movement;

	/** Alpha value for blending the flashlight rotation based on movement. */
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	float MovementAlpha = 0.f;

private:
	void CleanupComponent();

};

UENUM(BlueprintType)
enum class EFlashlightSocketRotationAxis : uint8
{
	Full			UMETA(DisplayName = "Full", ToolTip = "The full relative rotation of the socket will be used to influence the flashlight."),
	PitchOnly		UMETA(DisplayName = "Pitch Only", ToolTip = "Only the relative pitch rotation of the socket will be used to influence the flashlight. "),
};

UENUM(BlueprintType)
enum class EFlashlightSocketContext : uint8
{
	Chest			UMETA(DisplayName = "Chest", ToolTip = "We assume the flashlight is mounted to the chest, The flashlight will have additional configuration options for dealing with focus offset."),
	Head			UMETA(DisplayName = "Head", ToolTip = "We assume the flashlight is mounted to the head."),
	Camera			UMETA(DisplayName = "Camera", ToolTip = "We assume the flashlight is mounted to camera."),
};

UCLASS(BlueprintType, ClassGroup = PlayerCharacter)
class STORMWATCH_API UPlayerFlashlightConfiguration : public UDataAsset
{
	GENERATED_BODY()

public:

	/** The mounting position of the flashlight. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attachment", 
			  Meta = (DisplayName = "Attachment Context"))
	EFlashlightSocketContext AttachmentContext = EFlashlightSocketContext::Chest;

	/** The chest attachment socket. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attachment", 
			  Meta = (DisplayName = "Chest Socket", 
					  EditCondition = "AttachmentContext == EFlashlightSocketContext::Chest", 
					  EditConditionHides))
	FName ChestSocket = FName();

	/** The head attachment socket. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attachment", 
			  Meta = (DisplayName = "Head Socket",
					  EditCondition = "AttachmentContext == EFlashlightSocketContext::Head", 
					  EditConditionHides))
	FName HeadSocket = FName();

	/** When enabled, the flashlight will lag behind the player's orientation.*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Flashlight", 
			  Meta = (DisplayName = "Enable Rotation Lag", InlineEditConditionToggle))
	bool bRotationLagEnabled = true;

	/** The rotation lag of the flashlight. Lower values result in slower movement. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Flashlight", 
			  Meta = (DisplayName = "Rotation Lag", ClampMin = "0.0", ClampMax = "20.0", 
					  UIMin = "0.0", UIMax = "20.0", EditCondition = "bRotationLagEnabled"))
	float RotationLag = 8.5f;

	/** When enabled, the flashlight will sway according to the player's movement.*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Flashlight", 
			  Meta = (DisplayName = "Enable Rotation Sway", InlineEditConditionToggle))
	bool bSwayEnabled = true;

	/** The sway intensity of the flashlight. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Flashlight", 
			  Meta = (DisplayName = "Sway Intensity", ClampMin = "0.0", ClampMax = "2.0", 
					  UIMin = "0.0", UIMax = "2.0", EditCondition = "bSwayEnabled"))
	float RotationSway = 0.75f;

	/** When enabled, the flashlight rotation is influenced by the rotation of the socket that the flashlight springarm is attached to. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Flashlight", 
			  Meta = (DisplayName = "Enable Socket Rotation", 
					  EditCondition = "AttachmentContext != EFlashlightSocketContext::Camera"))
	bool bSocketRotationEnabled = true;

	/** Determines how much the flashlight orientation is affected by the skeletal mesh of the player character. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Flashlight", 
			  Meta = (DisplayName = "Socket Rotation Intensity", ClampMin = "0.0", ClampMax = "2.0", 
					  UIMin = "0.0", UIMax = "2.0", EditCondition = "bSocketRotationEnabled && AttachmentContext != EFlashlightSocketContext::Camera", 
					  EditConditionHides))
	float SocketRotation = 1.25f;

	/** When enabled, the flashlight will tilt into the lateral direction of the player. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Flashlight", 
			  Meta = (DisplayName = "Socket Rotation Axis", 
					  EditCondition = "bSocketRotationEnabled && AttachmentContext != EFlashlightSocketContext::Camera", 
					  EditConditionHides))
	EFlashlightSocketRotationAxis Axis = EFlashlightSocketRotationAxis::Full;

	/** When enabled, the flashlight will pitch to whatever object the player is looking at. This is only available if the flashlight is mounted on the chest of the character. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Flashlight", 
			  Meta = (DisplayName = "Enable Auto Focus", 
					  EditCondition = "AttachmentContext == EFlashlightSocketContext::Chest", 
					  EditConditionHides))
	bool bAutoFocusEnabled = true;

	/** The offset to use when the player is idle. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Flashlight|Offsets", 
			  Meta = (DisplayName = "Idle", AdvancedDisplay, 
					  EditCondition = "bSocketRotationEnabled && AttachmentContext != EFlashlightSocketContext::Camera", 
					  EditConditionHides))
	FVector2D IdleOffset = FVector2D();

	/** The offset to use when the player is walking. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Flashlight|Offsets", 
			  Meta = (DisplayName = "Walking", AdvancedDisplay, EditCondition = "bSocketRotationEnabled && AttachmentContext != EFlashlightSocketContext::Camera", EditConditionHides))
	FVector2D WalkingOffset = FVector2D();

	/** The offset to use when the player is sprinting. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Flashlight|Offsets", 
			  Meta = (DisplayName = "Sprinting", AdvancedDisplay, 
					  EditCondition = "bSocketRotationEnabled && AttachmentContext != EFlashlightSocketContext::Camera", 
					  EditConditionHides))
	FVector2D SprintingOffset = FVector2D();

	/** Determines how much the flashlight is allowed to orient up or down. Value is measures from the player character's forward vector. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Flashlight", 
			  Meta = (DisplayName = "Pitch Range", ClampMin = "0.0", ClampMax = "90.0", 
					  UIMin = "0.0", UIMax = "90.0"))
	float PitchRange = 60.0f;

	/** The intensity of the flashlight. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Light",
			  Meta = (DisplayName = "Intensity", ClampMin = "0.0"))
	float Intensity = 1.25f;

	/** The color of the flashlight. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Light", Meta = (DisplayName = "Light Color"))
	FColor LightColor = FColor(240,255,255,255);

	/** The attenuation radius of the flashlight. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Light", Meta = (DisplayName = "Attenuation Radius", ClampMin = "0.0"))
	float AttenuationRadius = 4000.0f;

	/** The inner cone angle of the flashlight. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Light", Meta = (DisplayName = "Inner Cone Angle", ClampMin = "0.0", ClampMax = "90.0", UIMin = "0.0", UIMax = "90.0"))
	float InnerConeAngle = 22.0f;

	/** The outer cone angle of the flashlight. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Light", Meta = (DisplayName = "Outer Cone Angle", ClampMin = "0.0", ClampMax = "90.0", UIMin = "0.0", UIMax = "90.0"))
	float OuterConeAngle = 34.0f;

	/** When true, the flashlight will casts shadows. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Shadows", Meta = (DisplayName = "Casts Shadows"))
	bool bCastShadows = false;

	/** The volumetric scattering intensity of the flashlight. It is recommended to keep this low to prevent blown out scenes. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "VolumetricScattering", Meta = (DisplayName = "Volumetric Scattering", ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float VolumetricScatteringIntensity = 0.0f;

	/** Whether the flashlight should use the inverse squared falloff formula for calculating light attenuation. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Fallof", Meta = (DisplayName = "Use Inverse Squared Falloff"))
	bool bUseInverseSquaredFalloff = false;

	/** The light falloff exponent of the flashlight. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Fallof", Meta = (DisplayName = "Light Falloff Exponent", ClampMin = "0.0", ClampMax = "8.0", UIMin = "0.0", UIMax = "8.0"))
	float LightFalloffExponent = 4.0f;

	/** The light function material for the flashlight. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "LightFunction", Meta = (DisplayName = "Light Function Material"))
	UMaterialInterface* LightFunctionMaterial;

	/** The light profile for the flashlight. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "LightProfile", Meta = (DisplayName = "IES Texture"))
	UTextureLightProfile* IESTexture;

	/** Constructor with default values. */
	UPlayerFlashlightConfiguration()
	{
	}

	/** Applies the flashlight configuration to a UFlashlightComponent instance. */
	void ApplyToFlashlightComponent(const UBHPlayerFlashlightComponent* Component);
};