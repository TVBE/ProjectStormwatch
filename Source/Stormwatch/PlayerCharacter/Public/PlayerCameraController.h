// Copyright (c) 2022-present Barrelhouse. All rights reserved.
// Written by Tim Verberne.

#pragma once

#include "PlayerCharacterComponent.h"
#include "UObject/WeakObjectPtr.h"
#include "PlayerCameraController.generated.h"

class UCameraComponent;
class APlayerCharacter;
class APlayerCharacterController;

struct CameraTickPacket
{
	UCameraComponent* Camera;

	const APlayerCharacter* Character;

	const APlayerCharacterController* Controller;

	const float DeltaTime;

	CameraTickPacket(UCameraComponent* InCamera, const APlayerCharacter* InCharacter, APlayerCharacterController* InController, float InDeltaTime)
		: Camera(InCamera)
		, Character(InCharacter)
		, Controller(InController)
		, DeltaTime(InDeltaTime)
	{
	}
};

/** UPlayerCameraController is an Actor Component responsible for managing the player camera's behavior, such as camera shakes and other effects.
 *	This class provides a simple and convenient way for designers to customize the camera's behavior and add special effects to the player's view. */
UCLASS(Blueprintable, BlueprintType, ClassGroup = "PlayerCharacter",
	   Meta = (BlueprintSpawnableComponent, DisplayName = "Player Camera Controller", ShortToolTip = "Camera controller for the player character."))
	class STORMWATCH_API UPlayerCameraController : public UPlayerCharacterComponent
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogPlayerCameraController, Log, All)

private:
	/** The default camera offset in relation to the Pawn's RootComponent. Use this to set the general camera position of the player in relation to their body. */
	UPROPERTY(EditDefaultsOnly, Category = "Camera",
			  Meta = (DisplayName = "Camera Offset"))
	FVector CameraOffset {FVector(22, 0, 75)};

	/** The default camera offset in relation to the Pawn's RootComponent when crouched. Use this to set the general camera position of the player in relation to their body. */
	UPROPERTY(EditDefaultsOnly, Category = "Camera",
			  Meta = (DisplayName = "Crouched Camera Offset"))
	FVector CrouchedCameraOffset {FVector(0, 0, 0)};

	/** Defines the maximum allowed camera angle. */
	UPROPERTY(EditDefaultsOnly, Category = "Camera",
			  Meta = (Displayname = "Maximum View Pitch", ClampMin = "0", ClampMax = "90", UiMin = "0", UIMax = "90"))
	float MaximumViewPitch {80.f};

	/** Defines the minimum allowed camera angle. */
	UPROPERTY(EditDefaultsOnly, Category = "Camera",
			  Meta = (Displayname = "Minimum View Pitch", ClampMin = "-90", ClampMax = "0", UiMin = "-90", UIMax = "0"))
	float MinimumViewPitch {-75.f};

	/** When enabled, the camera's field of view will scale according to the velocity of the player. This makes higher speeds seem more intense. */
	UPROPERTY(EditDefaultsOnly, Category = "Field Of View",
			  Meta = (DisplayName = "Enable Dynamic Field Of View"))
	bool IsDynamicFOVEnabled {true};

	/** The default FoV for the camera. */
	UPROPERTY(EditDefaultsOnly, Category = "Field Of View",
			  Meta = (DisplayName = "Default Field Of View", ClampMin = "60.0", ClampMax = "120.0", UIMin = "60.0", UIMax = "120.0"))
	float DefaultFOV {90.f};

	/** The FoV for the camera when the player is sprinting. Set this higher than the default FoV to get some perceived increase in speed for the player. */
	UPROPERTY(EditDefaultsOnly, Category = "Field Of View",
			  Meta = (DisplayName = "Sprint Field Of View", ClampMin = "60.0", ClampMax = "120.0", UIMin = "60.0", UIMax = "120.0", EditCondition = "IsDynamicFOVEnabled"))
	float SprintFOV {96.f};

	/** When enabled, depth of Field effects will be enabled for the camera. */
	UPROPERTY(EditDefaultsOnly, Category = "Depth Of Field",
			  Meta = (DisplayName = "Enable Dynamic Depth Of Field"))
	bool IsDynamicDOFEnabled {false};

	/** The minimum focus distance that the dynamic depth of field algorithm should enforce, in Unreal Units (cm). */
	UPROPERTY(EditDefaultsOnly, Category = "Depth Of Field",
			  Meta = (DisplayName = "Minimum Focal Distance", ClampMin = "0", ClampMax = "50000", UIMin = "0", UIMax = "50000", EditCondition = "IsDynamicDOFEnabled"))
	int MinimumFocalDistance {100};

	/** The maximum focus distance that the dynamic depth of field algorithm should enforce. */
	UPROPERTY(EditDefaultsOnly, Category = "Depth Of Field",
			  Meta = (DisplayName = "Maximum Focal Distance", ClampMin = "0", ClampMax = "50000", UIMin = "0", UIMax = "50000",
					  EditCondition = "IsDynamicDOFEnabled"))
	int MaximumFocalDistance {10000};

	/** The depth blur Focus at the smallest focal distance. */
	UPROPERTY(EditDefaultsOnly, Category = "Depth Of Field",
			  Meta = (DisplayName = "Macro Blur Focus", ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0", EditCondition = "IsDynamicDOFEnabled"))
	float MacroBlurFocus {0.05};

	/** The depth blur focus at the largest focal distance. */
	UPROPERTY(EditDefaultsOnly, Category = "Depth Of Field",
			  Meta = (DisplayName = "Long Shot Blur Focus", ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0", EditCondition = "IsDynamicDOFEnabled"))
	float LongShotBlurFocus {0.365};

	/** The depth blur Amount at the smallest focal distance. */
	UPROPERTY(EditDefaultsOnly, Category = "Depth Of Field",
			  Meta = (DisplayName = "Macro Blur Intensity", ClampMin = "0.0", ClampMax = "4.0", UIMin = "0.0", UIMax = "4.0", EditCondition = "IsDynamicDOFEnabled"))
	float MacroBlurAmount {1.4};

	/** The depth blur Amount at the largest focal distance. */
	UPROPERTY(EditDefaultsOnly, Category = "Depth Of Field",
			  Meta = (DisplayName = "Long Shot Blur Intensity", ClampMin = "0.0", ClampMax = "4.0", UIMin = "0.0", UIMax = "4.0", EditCondition = "IsDynamicDOFEnabled"))
	float LongShotBlurAmount {1.1};

	/** The interpolation speed for the dynamic depth of field algorithm*/
	UPROPERTY(EditDefaultsOnly, Category = "Depth Of Field",
			  Meta = (DisplayName = "Dynamic Depth Of Field Speed", ClampMin = "0.0", ClampMax = "10.0", UIMin = "0.0", UIMax = "10.0", EditCondition = "IsDynamicDOFEnabled"))
	float DynamicDofSpeed {6.75};

	/** When enabled, the vignette effect on the camera will be increased when sprinting, adding a bit of perceived intensity to the movement. */
	UPROPERTY(EditDefaultsOnly, Category = "Vignette",
			  Meta = (DisplayName = "Enabled Dynamic Vignette"))
	bool IsDynamicVignetteEnabled {false};

	/** The default vignette intensity for the camera. */
	UPROPERTY(EditDefaultsOnly, Category = "Vignette",
			  Meta = (DisplayName = "Default Vignette", ClampMin = "0.0", CLampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float DefaultVignetteIntensity {0.325};

	/** The intensity for the camera while sprinting. */
	UPROPERTY(EditDefaultsOnly, Category = "Vignette",
			  Meta = (DisplayName = "Sprint Vignette", ClampMin = "0.0", CLampMax = "1.0", UIMin = "0.0", UIMax = "1.0", EditCondition = "IsDynamicVignetteEnabled"))
	float SprintVignetteIntensity {0.45};

	/** When enabled, the camera will shake according to the movement op the player. Turn this off to reduce the potential of motion sickness for the user. */
	UPROPERTY(EditDefaultsOnly, Category = "Camera Sway",
			  Meta = (DisplayName = "Enable Camera Sway"))
	bool IsCameraSwayEnabled {true};

	/** The intensity of camera shake effects when moving. */
	UPROPERTY(EditDefaultsOnly, Category = "Camera Sway",
			  Meta = (DisplayName = "Camera Sway Intensity", ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0", EditCondition = "IsCameraSwayEnabled"))
	float CameraSwayIntensity {0.22};

	/** The influence of each axis of the camera's socket rotation on the camera sway. */
	UPROPERTY(EditDefaultsOnly, Category = "Camera Sway|Socket",
			  Meta = (DisplayName = "Socket Rotation Contribution While Upright", ClampMin = "0.0", ClampMax = "2.0", UIMin = "0.0", UIMax = "2.0", EditCondition = "IsCameraSwayEnabled"))
	FVector SocketRotationIntensity {FVector(1.0f, 1.5f, 0.0f)};

	/** The influence of each axis of the camera's socket rotation on the camera sway while crouched. */
	UPROPERTY(EditDefaultsOnly, Category = "Camera Sway|Socket",
			  Meta = (DisplayName = "Socket Rotation Contribution While Crouched", ClampMin = "0.0", ClampMax = "2.0", UIMin = "0.0", UIMax = "2.0", EditCondition = "IsCameraSwayEnabled"))
	FVector CrouchedSocketRotationIntensity {FVector(1.0f, 1.5f, 0.0f)};

	/** When enabled, the camera will lean into the direction of movement and rotation when the character is moving at high speeds.
	 *	This simulates the character leaning into the direction they're moving to balance out the centrifugal force when taking corners. */
	UPROPERTY(EditDefaultsOnly, Category = "Centripetal Rotation",
			  Meta = (DisplayName = "Enable Camera Centripetal Rotation"))
	bool IsCentripetalRotationEnabled {true};

	/** The maximum allowed centripetal rotation. */
	UPROPERTY(EditDefaultsOnly, Category = "Centripetal Rotation",
			  Meta = (DisplayName = "Max Centripetal Rotation Angle", ClampMin = "0.0", ClampMax = "45.0", UIMin = "0.0", UIMax = "45.0",
					  EditCondition = "IsCentripetalRotationEnabled"))
	float MaxCentripetalRotation {14.f};

	/** The intensity of the lean effect when the player is rotating their camera while sprinting. */
	UPROPERTY(EditDefaultsOnly, Category = "Centripetal Rotation",
			  Meta = (DisplayName = "Centripetal Rotation Intensity", ClampMin = "0.0", ClampMax = "4.0", UIMin = "0.0", UIMax = "4.0", EditCondition = "IsCentripetalRotationEnabled"))
	float CentripetalRotationIntensity {2.f};

	/** Multiplier for the centripetal rotation caused by lateral velocity of the player. */
	UPROPERTY(EditDefaultsOnly, Category = "Centripetal Rotation",
			  Meta = (DisplayName = "Velocity Based Centripetal Rotation", ClampMin = "0.0", ClampMax = "4.0", UIMin = "0.0", UIMax = "4.0", EditCondition = "IsCentripetalRotationEnabled"))
	float VelocityCentripetalRotation {1.f};

	/** Multiplier for the centripetal rotation caused by rotation of the player. */
	UPROPERTY(EditDefaultsOnly, Category = "Centripetal Rotation",
			  Meta = (DisplayName = "Rotation Based Centripetal Rotation", ClampMin = "0.0", ClampMax = "4.0", UIMin = "0.0", UIMax = "4.0", EditCondition = "IsCentripetalRotationEnabled"))
	float RotationCentripetalRotation {2.f};

	/** When enabled, centripetal rotation will also be applied when not sprinting. */
	UPROPERTY(EditDefaultsOnly, Category = "Centripetal Rotation",
			  Meta = (DisplayName = "Sprint Only", EditCondition = "IsCentripetalRotationEnabled"))
	bool IsCentripetalRotationSprintOnly {true};

	/** Multiplier for the centripetal rotation when not sprinting. */
	UPROPERTY(EditDefaultsOnly, Category = "Centripetal Rotation",
			  Meta = (DisplayName = "Sprint Multiplier", EditCondition = "IsCentripetalRotationEnabled"))
	float CentripetalRotationNonSprintMultiplier {1.25f};

	/** The default head socket rotation from the skeletal mesh of the PlayerCharacterPawn. */
	UPROPERTY()
	FTransform HeadSocketTransform {FTransform()};

	/** Interpolated head socket rotation. */
	UPROPERTY()
	FRotator InterpolatedHeadSocketRotation {FRotator()};

	/** The roll offset value of the camera shake rotation. */
	UPROPERTY()
	double CameraShakeRoll {0.0};

	/** The roll offset value of the camera lean rotation. */
	UPROPERTY()
	double CameraLeanRoll {0.0};

	/** The camera sway rotation. */
	UPROPERTY()
	FRotator Sway {FRotator()};

	/** The camera centripetal rotation. */
	UPROPERTY()
	FRotator CentripetalRotation {FRotator()};

	/** The camera socket rotation. */
	UPROPERTY()
	FRotator SocketRotation {FRotator()};

public:
	UPlayerCameraController();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Fades the camera from black. */
	void FadeFromBlack(const float Duration);

	/** Sets the maximum and minimum view pitch of the camera manager. */
	void SetCameraPitchLimits(TObjectPtr<APlayerCameraManager> CameraManager);

protected:
	virtual void OnRegister() override;

	virtual void BeginPlay() override;

private:
	/** Updates the camera relative location. */
	void UpdateCameraLocation(const CameraTickPacket& Packet);

	/** Updates the camera world rotation*/
	void UpdateCameraRotation(const CameraTickPacket& Packet);

	/** Returns a rotation offset for the camera to simulate the camera shaking while moving. */
	void GetCameraSwayRotation(const CameraTickPacket& Packet, FRotator& Rotator);

	/** Returns a rotation offset for the camera when the player rotates while sprinting. Used to simulate leaning when running into bends. */
	void GetCameraCentripetalRotation(const CameraTickPacket& Packet, FRotator& Rotator);

	/** Returns a scaled head socket delta rotation from the skeletal mesh of the PlayerCharacterPawn. */
	void GetScaledHeadSocketDeltaRotation(const CameraTickPacket& Packet, FRotator& Rotator);

	/** Updates the camera's field of view according to the Player's movement. */
	void UpdateCameraFieldOfView(const CameraTickPacket& Packet);

	/** Updates the camera's vignette intensity according to the Player's movement.*/
	void UpdateCameraVignetteIntensity(const CameraTickPacket& Packet);

	/** Updates the camera's depth of field according to whatever the player is looking at.*/
	void UpdateCameraDepthOfField(const CameraTickPacket& Packet);

	/** Performs a linetrace in the forward vector of the camera and returns the length of the trace. */
	float GetFocalDistance(UCameraComponent* Camera) const;
};
