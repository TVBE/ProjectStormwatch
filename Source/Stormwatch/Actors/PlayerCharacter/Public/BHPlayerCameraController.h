// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "BHPlayerCharacterSceneComponent.h"
#include "UObject/WeakObjectPtr.h"
#include "BHPlayerCameraController.generated.h"

class UCameraComponent;
class ABHPlayerCharacter;
class ABHPlayerCharacterController;

UCLASS(Blueprintable, BlueprintType, ClassGroup = "PlayerCharacter", Within = "PlayerCharacter")
	class STORMWATCH_API UBHPlayerCameraController : public UBHPlayerCharacterSceneComponent
{
	GENERATED_BODY()

public:
	UBHPlayerCameraController();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void FadeFromBlack(const float Duration);

	void SetCameraPitchLimits(TObjectPtr<APlayerCameraManager> CameraManager);

protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;

private:
	void OnControllerChanged(APawn* Pawn, AController* NewController, AController* PreviousController) override;

	void UpdateLocation(const float DeltaTime);
	void UpdateRotation(const float DeltaTime);
	void UpdateVignette(const float DeltaTime);
	void UpdateFOV(const float DeltaTime);
	void UpdateDOF(const float DeltaTime);
	float GetFocalDistance() const;

	FVector CalculateTargetLocation() const;
	float GetViewPitchAlpha() const;
	FVector GetHeadSocketZOffset() const;

	void AddCameraSwayRotation(FRotator& Rotator, const float DeltaTime);
	float GetCameraSwayIntensity() const;
	void AddCameraCentripetalRotation(FRotator& Rotator, const float DeltaTime);
	void AddScaledHeadSocketDeltaRotation(FRotator& Rotator, const float DeltaTime);

	UCameraComponent* GetCamera() const;
	FRotator GetControlRotation() const;

	/** Defines the maximum allowed camera angle. */
	UPROPERTY(EditDefaultsOnly, Category = "Camera",
	Meta = (Displayname = "Maximum View Pitch", ClampMin = "0", ClampMax = "90", UIMin = "0", UIMax = "90"))
	float MaximumViewPitch = 80.f;

	/** Defines the minimum allowed camera angle. */
	UPROPERTY(EditDefaultsOnly, Category = "Camera",
	Meta = (Displayname = "Minimum View Pitch", ClampMin = "-90", ClampMax = "0", UIMin = "-90", UIMax = "0"))
	float MinimumViewPitch = -75.f;

	/** When enabled, the camera's field of view will scale according to the velocity of the player.
	 *	This makes higher speeds seem more intense. */
	UPROPERTY(EditDefaultsOnly, Category = "Field Of View")
	bool bDynamicFOVEnabled = true;

	/** The default FoV for the camera. */
	UPROPERTY(EditDefaultsOnly, Category = "Field Of View",
	Meta = (DisplayName = "Default Field Of View", ClampMin = "60.0", ClampMax = "120.0", UIMin = "60.0", UIMax = "120.0"))
	float DefaultFOV = 90.f;

	/** The FoV for the camera when the player is sprinting.
	 *  Set this higher than the default FoV to get some perceived increase in speed for the player. */
	UPROPERTY(EditDefaultsOnly, Category = "Field Of View",
	Meta = (ClampMin = "60.0", ClampMax = "120.0", UIMin = "60.0", UIMax = "120.0", EditCondition = "bDynamicFOVEnabled"))
	float SprintFOV = 96.f;

	/** When enabled, depth of Field effects will be enabled for the camera. */
	UPROPERTY(EditDefaultsOnly, Category = "Depth Of Field", Meta = (DisplayName = "Enable Dynamic Depth Of Field"))
	bool bDynamicDOFEnabled = false;

	/** The minimum focus distance that the dynamic depth of field algorithm should enforce, in Unreal Units (cm). */
	UPROPERTY(EditDefaultsOnly, Category = "Depth Of Field",
	Meta = (ClampMin = "0", ClampMax = "50000", UIMin = "0", UIMax = "50000", EditCondition = "bDynamicDOFEnabled"))
	int MinimumFocalDistance = 100;

	/** The maximum focus distance that the dynamic depth of field algorithm should enforce. */
	UPROPERTY(EditDefaultsOnly, Category = "Depth Of Field",
	Meta = (ClampMin = "0", ClampMax = "50000", UIMin = "0", UIMax = "50000", EditCondition = "bDynamicDOFEnabled"))
	int MaximumFocalDistance = 10000;

	/** The depth blur Focus at the smallest focal distance. */
	UPROPERTY(EditDefaultsOnly, Category = "Depth Of Field",
	Meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0", EditCondition = "bDynamicDOFEnabled"))
	float MacroBlurFocus = 0.05;

	/** The depth blur focus at the largest focal distance. */
	UPROPERTY(EditDefaultsOnly, Category = "Depth Of Field",
	Meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0", EditCondition = "bDynamicDOFEnabled"))
	float LongShotBlurFocus = 0.365;

	/** The depth blur Amount at the smallest focal distance. */
	UPROPERTY(EditDefaultsOnly, Category = "Depth Of Field",
	Meta = (ClampMin = "0.0", ClampMax = "4.0", UIMin = "0.0", UIMax = "4.0", EditCondition = "bDynamicDOFEnabled"))
	float MacroBlurAmount = 1.4;

	/** The depth blur Amount at the largest focal distance. */
	UPROPERTY(EditDefaultsOnly, Category = "Depth Of Field",
	Meta = (ClampMin = "0.0", ClampMax = "4.0", UIMin = "0.0", UIMax = "4.0", EditCondition = "bDynamicDOFEnabled"))
	float LongShotBlurAmount = 1.1;

	/** The interpolation speed for the dynamic depth of field algorithm*/
	UPROPERTY(EditDefaultsOnly, Category = "Depth Of Field",
	Meta = (ClampMin = "0.0", ClampMax = "10.0", UIMin = "0.0", UIMax = "10.0", EditCondition = "bDynamicDOFEnabled"))
	float DynamicDofSpeed = 6.75;

	/** When enabled, the vignette effect on the camera will be increased when sprinting,
	 *	adding a bit of perceived intensity to the movement. */
	UPROPERTY(EditDefaultsOnly, Category = "Vignette")
	bool bDynamicVignetteEnabled = false;

	/** The default vignette intensity for the camera. */
	UPROPERTY(EditDefaultsOnly, Category = "Vignette",
	Meta = (DisplayName = "Default Vignette", ClampMin = "0.0", CLampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float DefaultVignetteIntensity = 0.325;

	/** The intensity for the camera while sprinting. */
	UPROPERTY(EditDefaultsOnly, Category = "Vignette",
	Meta = (ClampMin = "0.0", CLampMax = "1.0", UIMin = "0.0", UIMax = "1.0", EditCondition = "bDynamicVignetteEnabled"))
	float SprintVignetteIntensity = 0.45;

	/** When enabled, the camera will shake according to the movement op the player.
	 *	Turn this off to reduce the potential of motion sickness for the user. */
	UPROPERTY(EditDefaultsOnly, Category = "Camera Sway")
	bool bCameraSwayEnabled = true;

	/** The intensity of camera shake effects when moving. */
	UPROPERTY(EditDefaultsOnly, Category = "Camera Sway",
	Meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0", EditCondition = "bCameraSwayEnabled"))
	float CameraSwayIntensity = 0.22;

	/** The influence of each axis of the camera's socket rotation on the camera sway. */
	UPROPERTY(EditDefaultsOnly, Category = "Camera Sway|Socket",
	Meta = (ClampMin = "0.0", ClampMax = "2.0", UIMin = "0.0", UIMax = "2.0", EditCondition = "bCameraSwayEnabled"))
	FVector SocketRotationIntensity = FVector(1.0f, 1.5f, 0.0f);

	/** The influence of each axis of the camera's socket rotation on the camera sway while crouched. */
	UPROPERTY(EditDefaultsOnly, Category = "Camera Sway|Socket",
	Meta = (ClampMin = "0.0", ClampMax = "2.0", UIMin = "0.0", UIMax = "2.0", EditCondition = "bCameraSwayEnabled"))
	FVector CrouchedSocketRotationIntensity = FVector(1.0f, 1.5f, 0.0f);

	/** When enabled, the camera will lean into the direction of movement and rotation when the character is moving at high speeds.
	 *	This simulates the character leaning into the direction they're moving to balance out the centrifugal force when taking corners. */
	UPROPERTY(EditDefaultsOnly, Category = "Centripetal Rotation")
	bool bCentripetalRotationEnabled = true;

	/** The maximum allowed centripetal rotation. */
	UPROPERTY(EditDefaultsOnly, Category = "Centripetal Rotation",
	Meta = (ClampMin = "0.0", ClampMax = "45.0", UIMin = "0.0", UIMax = "45.0", EditCondition = "bCentripetalRotationEnabled"))
	float MaxCentripetalRotation = 14.f;

	/** The intensity of the lean effect when the player is rotating their camera while sprinting. */
	UPROPERTY(EditDefaultsOnly, Category = "Centripetal Rotation",
	Meta = (ClampMin = "0.0", ClampMax = "4.0", UIMin = "0.0", UIMax = "4.0", EditCondition = "bCentripetalRotationEnabled"))
	float CentripetalRotationIntensity = 2.f;

	/** Multiplier for the centripetal rotation caused by lateral velocity of the player. */
	UPROPERTY(EditDefaultsOnly, Category = "Centripetal Rotation",
	Meta = (ClampMin = "0.0", ClampMax = "4.0", UIMin = "0.0", UIMax = "4.0", EditCondition = "bCentripetalRotationEnabled"))
	float VelocityCentripetalRotation = 1.f;

	/** Multiplier for the centripetal rotation caused by rotation of the player. */
	UPROPERTY(EditDefaultsOnly, Category = "Centripetal Rotation",
	Meta = (ClampMin = "0.0", ClampMax = "4.0", UIMin = "0.0", UIMax = "4.0", EditCondition = "bCentripetalRotationEnabled"))
	float RotationCentripetalRotation = 2.f;

	/** When enabled, centripetal rotation will also be applied when not sprinting. */
	UPROPERTY(EditDefaultsOnly, Category = "Centripetal Rotation", Meta = (EditCondition = "bCentripetalRotationEnabled"))
	bool bCentripetalRotationSprintOnly = true;

	/** Multiplier for the centripetal rotation when not sprinting. */
	UPROPERTY(EditDefaultsOnly, Category = "Centripetal Rotation", Meta = (EditCondition = "bCentripetalRotationEnabled"))
	float CentripetalRotationNonSprintMultiplier = 1.25f;

	FTransform HeadSocketTransform = FTransform();
	
	FRotator InterpolatedHeadSocketRotation = FRotator();
	
	double CameraShakeRoll = 0.0f;
	double CameraLeanRoll = 0.0f;

	FVector ComponentLocationOrigin;
};
