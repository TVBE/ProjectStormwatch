// Copyright 2023 Tim Verberne

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacterConfiguration.generated.h"

/** Structure that defines the player characters configuration and behavior.
 *	This class provides a convenient way for designers to tweak the player settings, and can be stored inside a data asset.*/
USTRUCT(BlueprintType)
struct FPlayerCharacterConfigurationData
{
	GENERATED_USTRUCT_BODY()
	
	/** Defines the default movement speed.*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = General, Meta = (DisplayName = "Default Movement Speed", ClampMin = "0", ClampMax = "400", UiMin = "0", UIMax = "400"))
	float WalkSpeed {300.f};
	
	/** When set to enabled, the character is allowed to jump. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Jumping, Meta = (DisplayName = "Enable Jumping"))
	bool IsJumpingEnabled {true};

	/** Defines the jump velocity of the character. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Jumping, Meta = (DisplayName = "Jump Velocity", ClampMin = "0.0", UIMin = "0.0", ClampMax = "1000.0", UIMax = "1000.0"))
	float JumpVelocity {440.f};

	/** When set to enabled, the character is allowed to sprint. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Sprinting, Meta = (DisplayName = "Enable Sprinting"))
	bool IsSprintingEnabled {true};

	/** Defines the movement speed when sprinting. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Sprinting, Meta = (DisplayName = "Sprint Speed", EditCondition= "IsSprintingEnabled", ClampMin = "400", ClampMax = "800", UiMin = "400", UIMax = "800"))
	float SprintSpeed {600.f};

	/** When set to enabled, the character is allowed to crouch. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Crouching, Meta = (Displayname = "Enable Crouching"))
	bool IsCrouchingEnabled {true};

	/** Defines the characters movement speed while crouched. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Crouching, Meta = (Displayname = "Crouch Speed", EditCondition= "IsCrouchingEnabled", ClampMin = "0", ClampMax = "300", UiMin = "0", UIMax = "300"))
	float CrouchSpeed {200.f};

	/** When set to enabled, The crouch button will act as a toggle for the crouch action.
	 *	Enable this if you do not want the player to have to hold down the crouch button to remain crouched. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Crouching, Meta = (Displayname = "Crouch Toggle Mode", EditCondition= "IsCrouchingEnabled" , AdvancedDisplay = "true"))
	bool EnableCrouchToggle {false};
	
	/** Defines the rotation rate when using a gamepad.*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Camera, Meta = (Displayname = "Gamepad Rotation Rate"))
	float RotationRate {150.f};

	/** Defines the maximum allowed camera angle. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Camera, Meta = (Displayname = "Maximum View Pitch", ClampMin = "0", ClampMax = "90", UiMin = "0", UIMax = "90"))
	float MaximumViewPitch {80.f};

	/** Defines the minimum allowed camera angle. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Camera, Meta = (Displayname = "Minimum View Pitch", ClampMin = "-90", ClampMax = "0", UiMin = "-90", UIMax = "0"))
	float MinimumViewPitch {-75.f};
	
	/** Constructor with default values. */
	FPlayerCharacterConfigurationData()
	{
		ValidateData();
	}

	/** Checks some values to make sure that the character configuration is correct. */
	void ValidateData();

	/** Applies the character configuration to a PlayerCharacter instance. */
	void ApplyToPlayerCharacterInstance(class APlayerCharacter* PlayerCharacter);
};

/** Struct that defines the player's camera settings. */
USTRUCT(BlueprintType)
struct FPlayerCameraConfigurationData
{
	GENERATED_USTRUCT_BODY()

	/** The default camera offset in relation to the Pawn's RootComponent. Use this to set the general camera position of the player in relation to their body. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Camera, Meta = (DisplayName = "Camera Offset"))
	FVector CameraOffset {FVector(22, 0, 75)};

	/** When enabled, the camera's field of view will scale according to the velocity of the player. This makes higher speeds seem more intense. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = FieldOfView, Meta = (DisplayName = "Enable Dynamic Field Of View"))
	bool IsDynamicFOVEnabled {true};
	
	/** The default FoV for the camera. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = FieldOfView, Meta = (DisplayName = "Default Field Of View", ClampMin = "60.0", ClampMax = "120.0", UIMin = "60.0", UIMax = "120.0"))
	float DefaultFOV {90.f};

	/** The FoV for the camera when the player is sprinting. Set this higher than the default FoV to get some perceived increase in speed for the player. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = FieldOfView, Meta = (DisplayName = "Sprint Field Of View", ClampMin = "60.0", ClampMax = "120.0", UIMin = "60.0", UIMax = "120.0", EditCondition = "IsDynamicFOVEnabled"))
	float SprintFOV {96.f};

	/** When enabled, depth of Field effects will be enabled for the camera. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = DepthOfField, Meta = (DisplayName = "Enable Dynamic Depth Of Field"))
	bool IsDynamicDOFEnabled {false};
	
	/** The minimum focus distance that the dynamic depth of field algorithm should enforce, in Unreal Units (cm). */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = DepthOfField, Meta = (DisplayName = "Minimum Focal Distance", ClampMin = "0", ClampMax = "50000", UIMin = "0", UIMax = "50000", EditCondition = "IsDynamicDOFEnabled", EditConditionHides))
	int MinimumFocalDistance {0};
	
	/** The maximum focus distance that the dynamic depth of field algorithm should enforce. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = DepthOfField, Meta = (DisplayName = "Maximum Focal Distance", ClampMin = "0", ClampMax = "50000", UIMin = "0", UIMax = "50000", EditCondition = "IsDynamicDOFEnabled", EditConditionHides))
	int MaximumFocalDistance {25000};
	
	/** The depth blur Focus at the smallest focal distance. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = DepthOfField, Meta = (DisplayName = "Macro Blur Focus", ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0", EditCondition = "IsDynamicDOFEnabled", EditConditionHides))
	float MacroBlurFocus {0.65};
	
	/** The depth blur focus at the largest focal distance. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = DepthOfField, Meta = (DisplayName = "Long Shot Blur Focus", ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0", EditCondition = "IsDynamicDOFEnabled", EditConditionHides))
	float LongShotBlurFocus {0.2};

	/** The depth blur Amount at the smallest focal distance. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = DepthOfField, Meta = (DisplayName = "Macro Blur Intensity", ClampMin = "0.0", ClampMax = "4.0", UIMin = "0.0", UIMax = "4.0", EditCondition = "IsDynamicDOFEnabled", EditConditionHides))
	float MacroBlurAmount {0.65};
	
	/** The depth blur Amount at the largest focal distance. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = DepthOfField, Meta = (DisplayName = "Long Shot Blur Intensity", ClampMin = "0.0", ClampMax = "4.0", UIMin = "0.0", UIMax = "4.0", EditCondition = "IsDynamicDOFEnabled", EditConditionHides))
	float LongShotBlurAmount {0.2};
	
	/** The interpolation speed for the dynamic depth of field algorithm*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = DepthOfField, Meta = (DisplayName = "Dynamic Depth Of Field Speed", ClampMin = "0.0", ClampMax = "10.0", UIMin = "0.0", UIMax = "10.0", EditCondition = "IsDynamicDOFEnabled", EditConditionHides))
	float DynamicDofSpeed {2};
	
	/** When enabled, the camera will shake according to the movement op the player. Turn this off to reduce the potential of motion sickness for the user. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = CameraSway, Meta = (DisplayName = "Enable Camera Sway"))
	bool IsCameraSwayEnabled {true};

	/** The intensity of camera shake effects when moving. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = CameraSway, Meta = (DisplayName = "Camera Shake Intensity", ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float CameraShakeIntensity {0.22};

	/** When enabled, the camera will lean into the direction of movement and rotation when the character is moving at high speeds.
	 *	This simulates the character leaning into the direction they're moving to balance out the centrifugal force when taking corners. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = CentripetalRotation, Meta = (DisplayName = "Enable Camera Centripetal Rotation"))
	bool IsCentripetalRotationEnabled {true};

	/** The maximum allowed centripetal rotation. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = CentripetalRotation, Meta = (DisplayName = "Max Centripetal Rotation Angle", ClampMin = "0.0", ClampMax = "45.0", UIMin = "0.0", UIMax = "45.0", EditCondition = "IsCentripetalRotationEnabled", EditConditionHides))
	float MaxCentripetalRotation {14.f};

	/** The intensity of the lean effect when the player is rotating their camera while sprinting. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = CentripetalRotation, Meta = (DisplayName = "Centripetal Rotation Intensity", ClampMin = "0.0", ClampMax = "4.0", UIMin = "0.0", UIMax = "4.0", EditCondition = "IsCentripetalRotationEnabled", EditConditionHides))
	float CentripetalRotationIntensity {2.f};

	/** Multiplier for the centripetal rotation caused by lateral velocity of the player. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = CentripetalRotation, Meta = (DisplayName = "Velocity Based Centripetal Rotation", ClampMin = "0.0", ClampMax = "4.0", UIMin = "0.0", UIMax = "4.0",  EditCondition = "IsCentripetalRotationEnabled", EditConditionHides))
	float VelocityCentripetalRotation {1.f};

	/** Multiplier for the centripetal rotation caused by rotation of the player. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = CentripetalRotation, Meta = (DisplayName = "Rotation Based Centripetal Rotation", ClampMin = "0.0", ClampMax = "4.0", UIMin = "0.0", UIMax = "4.0", EditCondition = "IsCentripetalRotationEnabled", EditConditionHides))
	float RotationCentripetalRotation {2.f};	
 
	/** Constructor with default values. */
	FPlayerCameraConfigurationData()
	{
	}
 
};

UCLASS(BlueprintType)
class UPlayerCharacterConfiguration : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = PlayerCharacterConfiguration, Meta = (DisplayName = "Player Character Configuration"))
	FPlayerCharacterConfigurationData PlayerCharacterConfigurationData {FPlayerCharacterConfigurationData()};
};

UCLASS(BlueprintType)
class UPlayerCameraConfiguration : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = PlayerCameraConfiguration, Meta = (DisplayName = "Player Camera Configuration"))
	FPlayerCameraConfigurationData PlayerCameraConfigurationData {FPlayerCameraConfigurationData()};

	/** The original data on construction. Is used to be able to reset the configuration to its default values. */
	//UPROPERTY()
};

