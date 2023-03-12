// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacterConfiguration.generated.h"

class APlayerCharacter;
class UPlayerFlashlightComponent;
class UCameraComponent;

UCLASS(BlueprintType)
class UPlayerCharacterConfiguration : public UDataAsset
{
	GENERATED_BODY()

public:
	/** Defines the default movement speed.*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "General",
		Meta = (DisplayName = "Default Movement Speed", ClampMin = "0", ClampMax = "400", UiMin = "0", UIMax = "400"))
	float WalkSpeed {300.f};
	
	/** When set to enabled, the character is allowed to jump. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Jumping",
		Meta = (DisplayName = "Enable Jumping"))
	bool IsJumpingEnabled {true};

	/** Defines the jump velocity of the character. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Jumping",
		Meta = (DisplayName = "Jump Velocity", ClampMin = "0.0", UIMin = "0.0", ClampMax = "1000.0", UIMax = "1000.0"))
	float JumpVelocity {440.f};

	/** When set to enabled, the character is allowed to sprint. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Sprinting",
		Meta = (DisplayName = "Enable Sprinting"))
	bool IsSprintingEnabled {true};

	/** Defines the movement speed when sprinting. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Sprinting",
		Meta = (DisplayName = "Sprint Speed", EditCondition= "IsSprintingEnabled", ClampMin = "400", ClampMax = "800", UiMin = "400", UIMax = "800"))
	float SprintSpeed {600.f};

	/** When set to enabled, the character is allowed to crouch. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Crouching",
		Meta = (Displayname = "Enable Crouching"))
	bool IsCrouchingEnabled {true};

	/** Defines the characters movement speed while crouched. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Crouching",
		Meta = (Displayname = "Crouch Speed", EditCondition= "IsCrouchingEnabled", ClampMin = "0", ClampMax = "300", UiMin = "0", UIMax = "300"))
	float CrouchSpeed {200.f};

	/** When set to enabled, The crouch button will act as a toggle for the crouch action.
	 *	Enable this if you do not want the player to have to hold down the crouch button to remain crouched. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Crouching",
		Meta = (Displayname = "Crouch Toggle Mode", EditCondition= "IsCrouchingEnabled" , AdvancedDisplay = "true"))
	bool EnableCrouchToggle {false};
	
	/** Defines the rotation rate when using a gamepad.*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Camera",
		Meta = (Displayname = "Gamepad Rotation Rate"))
	float RotationRate {150.f};
	
	/** Constructor with default values. */
	UPlayerCharacterConfiguration()
	{
	}
	
	/** Applies the character configuration to a PlayerCharacter instance. */
	void ApplyToPlayerCharacter(const APlayerCharacter* PlayerCharacter);

	/** Applies some values of the character configuration to the player controller and it's corresponding camera manager. */
	void ApplyToPlayerController(APlayerController* PlayerController);
	
};

UCLASS(BlueprintType)
class UPlayerCameraConfiguration : public UDataAsset
{
	GENERATED_BODY()

public:
	/** The default camera offset in relation to the Pawn's RootComponent. Use this to set the general camera position of the player in relation to their body. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Camera",
		Meta = (DisplayName = "Camera Offset"))
	FVector CameraOffset {FVector(22, 0, 75)};

	/** Defines the maximum allowed camera angle. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Camera",
		Meta = (Displayname = "Maximum View Pitch", ClampMin = "0", ClampMax = "90", UiMin = "0", UIMax = "90"))
	float MaximumViewPitch {80.f};

	/** Defines the minimum allowed camera angle. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Camera",
		Meta = (Displayname = "Minimum View Pitch", ClampMin = "-90", ClampMax = "0", UiMin = "-90", UIMax = "0"))
	float MinimumViewPitch {-75.f};

	/** When enabled, the camera's field of view will scale according to the velocity of the player. This makes higher speeds seem more intense. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FieldOfView",
		Meta = (DisplayName = "Enable Dynamic Field Of View"))
	bool IsDynamicFOVEnabled {true};
	
	/** The default FoV for the camera. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FieldOfView",
		Meta = (DisplayName = "Default Field Of View", ClampMin = "60.0", ClampMax = "120.0", UIMin = "60.0", UIMax = "120.0"))
	float DefaultFOV {90.f};

	/** The FoV for the camera when the player is sprinting. Set this higher than the default FoV to get some perceived increase in speed for the player. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FieldOfView",
		Meta = (DisplayName = "Sprint Field Of View", ClampMin = "60.0", ClampMax = "120.0", UIMin = "60.0", UIMax = "120.0", EditCondition = "IsDynamicFOVEnabled"))
	float SprintFOV {96.f};

	/** When enabled, depth of Field effects will be enabled for the camera. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "DepthOfField",
		Meta = (DisplayName = "Enable Dynamic Depth Of Field"))
	bool IsDynamicDOFEnabled {false};
	
	/** The minimum focus distance that the dynamic depth of field algorithm should enforce, in Unreal Units (cm). */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "DepthOfField",
		Meta = (DisplayName = "Minimum Focal Distance", ClampMin = "0", ClampMax = "50000", UIMin = "0", UIMax = "50000", EditCondition = "IsDynamicDOFEnabled", EditConditionHides))
	int MinimumFocalDistance {100};
	
	/** The maximum focus distance that the dynamic depth of field algorithm should enforce. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "DepthOfField",
		Meta = (DisplayName = "Maximum Focal Distance", ClampMin = "0", ClampMax = "50000", UIMin = "0", UIMax = "50000", EditCondition = "IsDynamicDOFEnabled", EditConditionHides))
	int MaximumFocalDistance {10000};
	
	/** The depth blur Focus at the smallest focal distance. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "DepthOfField",
		Meta = (DisplayName = "Macro Blur Focus", ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0", EditCondition = "IsDynamicDOFEnabled", EditConditionHides))
	float MacroBlurFocus {0.05};
	
	/** The depth blur focus at the largest focal distance. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "DepthOfField",
		Meta = (DisplayName = "Long Shot Blur Focus", ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0", EditCondition = "IsDynamicDOFEnabled", EditConditionHides))
	float LongShotBlurFocus {0.365};

	/** The depth blur Amount at the smallest focal distance. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "DepthOfField",
		Meta = (DisplayName = "Macro Blur Intensity", ClampMin = "0.0", ClampMax = "4.0", UIMin = "0.0", UIMax = "4.0", EditCondition = "IsDynamicDOFEnabled", EditConditionHides))
	float MacroBlurAmount {1.4};
	
	/** The depth blur Amount at the largest focal distance. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "DepthOfField",
		Meta = (DisplayName = "Long Shot Blur Intensity", ClampMin = "0.0", ClampMax = "4.0", UIMin = "0.0", UIMax = "4.0", EditCondition = "IsDynamicDOFEnabled", EditConditionHides))
	float LongShotBlurAmount {1.1};
	
	/** The interpolation speed for the dynamic depth of field algorithm*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "DepthOfField",
		Meta = (DisplayName = "Dynamic Depth Of Field Speed", ClampMin = "0.0", ClampMax = "10.0", UIMin = "0.0", UIMax = "10.0", EditCondition = "IsDynamicDOFEnabled", EditConditionHides))
	float DynamicDofSpeed {6.75};

	/** When enabled, the vignette effect on the camera will be increased when sprinting, adding a bit of perceived intensity to the movement. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Vignette",
		Meta = (DisplayName = "Enabled Dynamic Vignette"))
	bool IsDynamicVignetteEnabled {false};

	/** The default vignette intensity for the camera. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Vignette",
		Meta = (DisplayName = "Default Vignette", ClampMin = "0.0", CLampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float DefaultVignetteIntensity {0.325};

	/** The intensity for the camera while sprinting. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Vignette",
		Meta = (DisplayName = "Sprint Vignette", ClampMin = "0.0", CLampMax = "1.0", UIMin = "0.0", UIMax = "1.0", EditCondition = "IsDynamicVignetteEnabled", EditConditionHides))
	float SprintVignetteIntensity {0.45};
	
	/** When enabled, the camera will shake according to the movement op the player. Turn this off to reduce the potential of motion sickness for the user. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "CameraSway",
		Meta = (DisplayName = "Enable Camera Sway"))
	bool IsCameraSwayEnabled {true};

	/** The intensity of camera shake effects when moving. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "CameraSway",
		Meta = (DisplayName = "Camera Shake Intensity", ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float CameraShakeIntensity {0.22};

	/** When enabled, the camera will lean into the direction of movement and rotation when the character is moving at high speeds.
	 *	This simulates the character leaning into the direction they're moving to balance out the centrifugal force when taking corners. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "CentripetalRotation",
		Meta = (DisplayName = "Enable Camera Centripetal Rotation"))
	bool IsCentripetalRotationEnabled {true};

	/** The maximum allowed centripetal rotation. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "CentripetalRotation",
		Meta = (DisplayName = "Max Centripetal Rotation Angle", ClampMin = "0.0", ClampMax = "45.0", UIMin = "0.0", UIMax = "45.0", EditCondition = "IsCentripetalRotationEnabled", EditConditionHides))
	float MaxCentripetalRotation {14.f};

	/** The intensity of the lean effect when the player is rotating their camera while sprinting. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "CentripetalRotation",
		Meta = (DisplayName = "Centripetal Rotation Intensity", ClampMin = "0.0", ClampMax = "4.0", UIMin = "0.0", UIMax = "4.0", EditCondition = "IsCentripetalRotationEnabled", EditConditionHides))
	float CentripetalRotationIntensity {2.f};

	/** Multiplier for the centripetal rotation caused by lateral velocity of the player. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "CentripetalRotation",
		Meta = (DisplayName = "Velocity Based Centripetal Rotation", ClampMin = "0.0", ClampMax = "4.0", UIMin = "0.0", UIMax = "4.0",  EditCondition = "IsCentripetalRotationEnabled", EditConditionHides))
	float VelocityCentripetalRotation {1.f};

	/** Multiplier for the centripetal rotation caused by rotation of the player. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "CentripetalRotation",
		Meta = (DisplayName = "Rotation Based Centripetal Rotation", ClampMin = "0.0", ClampMax = "4.0", UIMin = "0.0", UIMax = "4.0", EditCondition = "IsCentripetalRotationEnabled", EditConditionHides))
	float RotationCentripetalRotation {2.f};	
 
	/** Constructor with default values. */
	UPlayerCameraConfiguration()
	{
	}
	
	/** Applies the camera configuration to a PlayerCharacter instance. */
	void ApplyToCamera(UCameraComponent* Camera);
};

/** Enumeration for defining the socket rotation axis. */
UENUM(BlueprintType)
enum class EFlashlightSocketRotationAxis : uint8
{
	Full			UMETA(DisplayName = "Full", ToolTip = "The full relative rotation of the socket will be used to influence the flashlight."),
	PitchOnly		UMETA(DisplayName = "Pitch Only", ToolTip = "Only the relative pitch rotation of the socket will be used to influence the flashlight. "),
};

/** Enumeration for defining the mounting position of the flashlight. */
UENUM(BlueprintType)
enum class EFlashlightSocketContext : uint8
{
	Chest			UMETA(DisplayName = "Chest", ToolTip = "We assume the flashlight is mounted to the chest, The flashlight will have additional configuration options for dealing with focus offset."),
	Head			UMETA(DisplayName = "Head", ToolTip = "We assume the flashlight is mounted to the head."),
	Camera			UMETA(DisplayName = "Camera", ToolTip = "We assume the flashlight is mounted to camera."),
};


UCLASS(BlueprintType)
class UPlayerFlashlightConfiguration : public UDataAsset
{
	GENERATED_BODY()

public:
	
	/** The mounting position of the flashlight. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attachment", Meta = (DisplayName = "Attachment Context"))
	EFlashlightSocketContext AttachmentContext {EFlashlightSocketContext::Chest};
	
	/** The chest attachment socket. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attachment", Meta = (DisplayName = "Chest Socket",
		EditCondition = "AttachmentContext == EFlashlightSocketContext::Chest", EditConditionHides))
	FName ChestSocket {FName()};

	/** The head attachment socket. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attachment", Meta = (DisplayName = "Head Socket",
		EditCondition = "AttachmentContext == EFlashlightSocketContext::Head", EditConditionHides))
	FName HeadSocket {FName()};
	
	/** When enabled, the flashlight will lag behind the player's orientation.*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Flashlight", Meta = (DisplayName = "Enable Rotation Lag", InlineEditConditionToggle))
	bool IsRotationLagEnabled {true};

	/** The rotation lag of the flashlight. Lower values result in slower movement. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Flashlight", Meta = (DisplayName = "Rotation Lag",
		ClampMin = "0.0", ClampMax = "20.0", UIMin = "0.0", UIMax = "20.0", EditCondition = "IsRotationLagEnabled"))
	float RotationLag {8.5f};

	/** When enabled, the flashlight will sway according to the player's movement.*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Flashlight", Meta = (DisplayName = "Enable Rotation Sway", InlineEditConditionToggle))
	bool IsSwayEnabled {true};

	/** The sway intensity of the flashlight. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Flashlight", Meta = (DisplayName = "Sway Intensity",
		ClampMin = "0.0", ClampMax = "2.0", UIMin = "0.0", UIMax = "2.0", EditCondition = "IsSwayEnabled"))
	float RotationSway {0.75f};

	/** When enabled, the flashlight rotation is influenced by the rotation of the socket that the flashlight springarm is attached to. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Flashlight", Meta = (DisplayName = "Enable Socket Rotation",
		EditCondition = "AttachmentContext != EFlashlightSocketContext::Camera"))
	bool IsSocketRotationEnabled {true};
	
	/** Determines how much the flashlight orientation is affected by the skeletal mesh of the player character. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Flashlight", Meta = (DisplayName = "Socket Rotation Intensity",
		ClampMin = "0.0", ClampMax = "2.0", UIMin = "0.0", UIMax = "2.0",
		EditCondition = "IsSocketRotationEnabled && AttachmentContext != EFlashlightSocketContext::Camera", EditConditionHides))
	float SocketRotation {1.25f};

	/** When enabled, the flashlight will tilt into the lateral direction of the player. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Flashlight", Meta = (DisplayName = "Socket Rotation Axis",
		EditCondition = "IsSocketRotationEnabled && AttachmentContext != EFlashlightSocketContext::Camera", EditConditionHides))
	EFlashlightSocketRotationAxis Axis {EFlashlightSocketRotationAxis::Full};

	/** When enabled, the flashlight will pitch to whatever object the player is looking at. This is only available if the flashlight is mounted on the chest of the character. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Flashlight", Meta = (DisplayName = "Enable Auto Focus",
		EditCondition = "AttachmentContext == EFlashlightSocketContext::Chest", EditConditionHides))
	bool IsAutoFocusEnabled {true};

	/** The offset to use when the player is idle. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Flashlight|Offsets", Meta = (DisplayName = "Idle",
		AdvancedDisplay, EditCondition = "IsSocketRotationEnabled && AttachmentContext != EFlashlightSocketContext::Camera", EditConditionHides))
	FVector2D IdleOffset {FVector2D()};

	/** The offset to use when the player is walking. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Flashlight|Offsets", Meta = (DisplayName = "Walking",
		AdvancedDisplay, EditCondition = "IsSocketRotationEnabled && AttachmentContext != EFlashlightSocketContext::Camera", EditConditionHides))
	FVector2D WalkingOffset {FVector2D()};

	/** The offset to use when the player is sprinting. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Flashlight|Offsets", Meta = (DisplayName = "Sprinting",
		AdvancedDisplay, EditCondition = "IsSocketRotationEnabled && AttachmentContext != EFlashlightSocketContext::Camera", EditConditionHides))
	FVector2D SprintingOffset {FVector2D()};
	
	/** Determines how much the flashlight is allowed to orient up or down. Value is measures from the player character's forward vector. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Flashlight", Meta = (DisplayName = "Pitch Range", ClampMin = "0.0", ClampMax = "90.0", UIMin = "0.0", UIMax = "90.0"))
	float PitchRange {60.0f};
	
	
	/** The intensity of the flashlight. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Light",
		Meta = (DisplayName = "Intensity", ClampMin = "0.0"))
	float Intensity {1.25f};

	/** The color of the flashlight. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Light", Meta = (DisplayName = "Light Color"))
	FColor LightColor {FColor(240,255,255,255)};

	/** The attenuation radius of the flashlight. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Light", Meta = (DisplayName = "Attenuation Radius", ClampMin = "0.0"))
	float AttenuationRadius {4000.0f};

	/** The inner cone angle of the flashlight. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Light", Meta = (DisplayName = "Inner Cone Angle", ClampMin = "0.0", ClampMax = "90.0", UIMin = "0.0", UIMax = "90.0"))
	float InnerConeAngle {22.0f};

	/** The outer cone angle of the flashlight. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Light", Meta = (DisplayName = "Outer Cone Angle", ClampMin = "0.0", ClampMax = "90.0", UIMin = "0.0", UIMax = "90.0"))
	float OuterConeAngle {34.0f};

	/** When true, the flashlight will casts shadows. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Shadows", Meta = (DisplayName = "Casts Shadows"))
	bool CastsShadows {false};

	/** The volumetric scattering intensity of the flashlight. It is recommended to keep this low to prevent blown out scenes. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "VolumetricScattering", Meta = (DisplayName = "Volumetric Scattering", ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float VolumetricScatteringIntensity {0.0f};

	/** Whether the flashlight should use the inverse squared falloff formula for calculating light attenuation. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Fallof", Meta = (DisplayName = "Use Inverse Squared Falloff"))
	bool UseInverseSquaredFalloff {false};

	/** The light falloff exponent of the flashlight. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Fallof", Meta = (DisplayName = "Light Falloff Exponent", ClampMin = "0.0", ClampMax = "8.0", UIMin = "0.0", UIMax = "8.0"))
	float LightFalloffExponent {4.0f};

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
	void ApplyToFlashlightComponent(const UPlayerFlashlightComponent* Component);
	
};

UCLASS(BlueprintType)
class UPlayerStateConfiguration : public UDataAsset
{
	GENERATED_BODY()

public:
	/** The amount of health that is restored every update interval. */
	UPROPERTY(BlueprintReadOnly, Category = "Health", Meta = (DisplayName = "Health Regeneration"))
	uint8 HealthRegenAmount {2};
	
	/** The amount of exertion that is removed per update interval when the player is not performing intensive movements.*/
	UPROPERTY(BlueprintReadOnly, Category = "Exertion", Meta = (DisplayName = "Exertion Reduction Amount"))
	uint8 ExertionReductionAmount {2};
	
	/** The amount of exertion that is added per update interval when the player sprints. */
	UPROPERTY(BlueprintReadOnly, Category = "Exertion", Meta = (DisplayName = "Sprint Increment"))
	uint8 SprintExertionIncrement {3};

	/** The amount of exertion that is added when the player jumps. */
	UPROPERTY(BlueprintReadOnly, Category = "Exertion", Meta = (DisplayName = "Jump Increment"))
	uint8 JumpExertionIncrement {5};

	/** Constructor with default values. */
	UPlayerStateConfiguration()
	{
	}

	/** Applies some values of the camera configuration to the player controller. */
	void ApplyToPlayerController(APlayerController* PlayerController);
};
