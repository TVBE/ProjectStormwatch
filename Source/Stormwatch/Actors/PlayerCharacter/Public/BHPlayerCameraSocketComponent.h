// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "BHPlayerCharacterComponent.h"
#include "UObject/WeakObjectPtr.h"
#include "BHPlayerCameraSocketComponent.generated.h"

class UCameraComponent;
class ABHPlayerCharacter;
class ABHPlayerController;

// Scene component for attaching a camera to in the character. Contains logic for adding camera movement effects such as
// camera sway and Centripetal Roll. Does not apply any image effects to the camera itself.
// Movement effects are applied directly to this component, which is propagated to anything attached to this component.
// This means that it is unsafe to manually set the transform of this component during runtime.
UCLASS(NotBlueprintable, BlueprintType, ClassGroup = "Barrelhouse")
	class STORMWATCH_API UBHPlayerCameraSocketComponent : public USceneComponent, public FBHPlayerCharacterComponent
{
	GENERATED_BODY()

public:
	UBHPlayerCameraSocketComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;
	virtual void OnRegister() override;

private:
	void UpdateLocation(float DeltaTime);
	void UpdateRotation(float DeltaTime);

	void AddCameraSway(FRotator& Rotator, float DeltaTime);
	void AddCentripetalRoll(FRotator& Rotator, float DeltaTime);
	void AddSocketSway(FRotator& Rotator, float DeltaTime);
	FVector CalculateTargetLocation() const;
	float GetViewPitchAlpha() const;
	FVector GetHeadSocketZOffset() const;
	float GetCameraSwayIntensity() const;
	
	FRotator GetControlRotation() const;
	
	// The socket we'll use as our attachment point.
	UPROPERTY(EditDefaultsOnly, Category = "Socket", Meta = (DisplayName = "Socket"))
	FName SocketName = NAME_None;
	
	// Defines the maximum allowed camera angle.
	UPROPERTY(EditDefaultsOnly, Category = "Camera",
	Meta = (Displayname = "Maximum View Pitch", ClampMin = "0", ClampMax = "90", UIMin = "0", UIMax = "90"))
	float MaximumViewPitch = 80.f;

	// Defines the minimum allowed camera angle.
	UPROPERTY(EditDefaultsOnly, Category = "Camera",
	Meta = (Displayname = "Minimum View Pitch", ClampMin = "-90", ClampMax = "0", UIMin = "-90", UIMax = "0"))
	float MinimumViewPitch = -75.f;
	
	// When enabled, the camera will shake according to the movement op the player.
	//Turn this off to reduce the potential of motion sickness for the user.
	UPROPERTY(EditDefaultsOnly, Category = "Camera Sway")
	bool bCameraSwayEnabled = true;

	// The intensity of camera shake effects when moving.
	UPROPERTY(EditDefaultsOnly, Category = "Camera Sway",
	Meta = (ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1", EditCondition = "bCameraSwayEnabled"))
	float CameraSwayIntensity = 0.22f;

	UPROPERTY(EditDefaultsOnly, Category = "Camera Sway",
	Meta = (XAxisName="Speed", YAxisName="Intensity", DisplayName = "Intensity Over Velocity"))
	FRuntimeFloatCurve CameraSwayIntensityCurve = {};

	UPROPERTY(EditDefaultsOnly, Category = "Socket Sway")
	bool bSocketSwayEnabled = true;
	
	// The influence of each axis of the camera's socket rotation on the camera sway.
	UPROPERTY(EditDefaultsOnly, Category = "Socket Sway",
	Meta = (ClampMin = "0", ClampMax = "2", UIMin = "0", UIMax = "2", EditCondition = "bSocketSwayEnabled"))
	FVector SocketSwayIntensity {1.0f, 1.5f, 0.0f};

	// The influence of each axis of the camera's socket rotation on the camera sway while crouched.
	UPROPERTY(EditDefaultsOnly, Category = "Socket Sway",
	Meta = (ClampMin = "0", ClampMax = "2", UIMin = "0", UIMax = "2", EditCondition = "bSocketSwayEnabled"))
	FVector CrouchedSocketSwayIntensity {1.0f, 1.5f, 0.0f};

	UPROPERTY(EditDefaultsOnly, Category = "Centripetal Roll",
	Meta = (ClampMin = "1", UIMin = "1", EditCondition = "bCentripetalRollEnabled"))
	float SocketSwaySpeed = 4.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Socket Sway",
	Meta = (XAxisName="Speed", YAxisName="Intensity", EditCondition = "bSocketSwayEnabled",
			DisplayName = "Intensity Over Velocity"))
	FRuntimeFloatCurve SocketSwayIntensityCurve {};

	// The range in which we start locking the camera to the character's mesh camera socket when looking down.
	UPROPERTY(EditDefaultsOnly, Category = "Socket Sway",
	Meta = (ClampMin = "-90", ClampMax = "0", UIMin = "-90", UIMax = "0", EditCondition = "bCameraSwayEnabled"))
	FVector2D SocketLockPitchTransitionRange = {-30.0f, -55.0f};

	// Causes the camera to lean into the direction of movement or rotation to mimic the effect of balancing out
	// centrifugal forces. Centripetal roll can either be driven by camera rotation or character movement.
	UPROPERTY(EditDefaultsOnly, Category = "Centripetal Roll", Meta = (DisplayName = "Enable Centripetal Roll"))
	bool bCentripetalRollEnabled = true;

	// The maximum allowed Centripetal Roll.
	UPROPERTY(EditDefaultsOnly, Category = "Centripetal Roll",
	Meta = (Units = "Degrees", ClampMin = "0", ClampMax = "45", UIMin = "0", UIMax = "45",
			EditCondition = "bCentripetalRollEnabled"))
	float MaxCentripetalRoll = 14.f;

	// Intensity of the centripetal roll when the player is rotating their camera..
	UPROPERTY(EditDefaultsOnly, Category = "Centripetal Roll",
	Meta = (ClampMin = "0", ClampMax = "4", UIMin = "0", UIMax = "4", EditCondition = "bCentripetalRollEnabled"))
	float RotationDrivenCentripetalRollIntensity = 2.f;

	// Intensity of the centripetal roll caused by lateral velocity of the player.
	UPROPERTY(EditDefaultsOnly, Category = "Centripetal Roll",
	Meta = (ClampMin = "0", ClampMax = "4", UIMin = "0", UIMax = "4", EditCondition = "bCentripetalRollEnabled"))
	float MovementDrivenCentripetalRollIntensity = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Centripetal Roll",
	Meta = (ClampMin = "1", UIMin = "1", EditCondition = "bCentripetalRollEnabled"))
	float CentripetalRollSpeed = 4.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Centripetal Roll",
	Meta = (XAxisName="Speed", YAxisName="Intensity", EditCondition = "bCentripetalRollEnabled",
			DisplayName = "Intensity over Velocity"))
	FRuntimeFloatCurve RotationDrivenCentripetalRollIntensityCurve = {};

	UPROPERTY(EditDefaultsOnly, Category = "Centripetal Roll",
	Meta = (XAxisName="Speed", YAxisName="Intensity", EditCondition = "bCentripetalRollEnabled",
			DisplayName = "Intensity over Velocity"))
	FRuntimeFloatCurve MovementDrivenCentripetalRollIntensityCurve = {};
	
	FTransform RelativeTransform = FTransform::Identity;
	
	FRotator InterpolatedHeadSocketRotation = FRotator::ZeroRotator;
	
	double CameraShakeRoll = 0.0f;
	double CameraLeanRoll = 0.0f;
};
