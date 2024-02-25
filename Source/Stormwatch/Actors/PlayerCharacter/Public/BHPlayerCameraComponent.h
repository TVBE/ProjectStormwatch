// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "Camera/CameraComponent.h"
#include "BHPlayerCharacterComponent.h"
#include "BHPlayerCameraComponent.generated.h"

USTRUCT(BlueprintType)
struct FBHCameraQuery
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly)
	FHitResult HitResult;

	UPROPERTY(BlueprintReadOnly)
	float TraceLength = 0.0f;
};

// Camera component for the player character. Can apply dynamic image effects like Vignette and DOF based on the character's movement.
// Also performs a camera trace every second in the view direction of the component.
// Access this component to get information regarding objects the player is currently looking at.
UCLASS(NotBlueprintable, BlueprintType, ClassGroup = "Barrelhouse")
class STORMWATCH_API UBHPlayerCameraComponent : public UCameraComponent, public FBHPlayerCharacterComponent
{
	GENERATED_BODY()
	
public:
	UBHPlayerCameraComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintPure, Category = "BHCameraComponent")
	const FBHCameraQuery& GetCameraTrace() const;

protected:
	virtual void PostLoad() override;

	virtual void OnRegister() override;
	virtual void OnUnregister() override;

private:
	void UpdateVignette(const float DeltaTime);
	void UpdateFOV(const float DeltaTime);
	void UpdateDOF(const float DeltaTime);
	float GetFocalDistance() const;

	// The maximum length to perform a camera trace with.
	UPROPERTY(EditDefaultsOnly, Category = "Camera Query")
	float CameraTraceLength = 10000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Camera Query")
	bool bTraceComplex = false;
	
	// When enabled, the camera's field of view will scale according to the velocity of the player.
	// This makes higher speeds seem more intense.
	UPROPERTY(EditDefaultsOnly, Category = "Field Of View")
	bool bDynamicFOVEnabled = true;

	// The default FoV for the camera.
	UPROPERTY(EditDefaultsOnly, Category = "Field Of View",
	Meta = (DisplayName = "Default Field Of View", ClampMin = "60.0", ClampMax = "120.0", UIMin = "60.0", UIMax = "120.0"))
	float DefaultFOV = 90.f;

	// The FoV for the camera when the player is sprinting.
	//  Set this higher than the default FoV to get some perceived increase in speed for the player.
	UPROPERTY(EditDefaultsOnly, Category = "Field Of View",
	Meta = (ClampMin = "60.0", ClampMax = "120.0", UIMin = "60.0", UIMax = "120.0", EditCondition = "bDynamicFOVEnabled"))
	float SprintFOV = 96.f;

	// When enabled, depth of Field effects will be enabled for the camera.
	UPROPERTY(EditDefaultsOnly, Category = "Depth Of Field", Meta = (DisplayName = "Enable Dynamic Depth Of Field"))
	bool bDynamicDOFEnabled = false;

	// The minimum focus distance that the dynamic depth of field algorithm should enforce, in Unreal Units (cm).
	UPROPERTY(EditDefaultsOnly, Category = "Depth Of Field",
	Meta = (ClampMin = "0", ClampMax = "50000", UIMin = "0", UIMax = "50000", EditCondition = "bDynamicDOFEnabled"))
	int MinimumFocalDistance = 100;

	// The maximum focus distance that the dynamic depth of field algorithm should enforce.
	UPROPERTY(EditDefaultsOnly, Category = "Depth Of Field",
	Meta = (ClampMin = "0", ClampMax = "50000", UIMin = "0", UIMax = "50000", EditCondition = "bDynamicDOFEnabled"))
	int MaximumFocalDistance = 10000;

	// The depth blur Focus at the smallest focal distance.
	UPROPERTY(EditDefaultsOnly, Category = "Depth Of Field",
	Meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0", EditCondition = "bDynamicDOFEnabled"))
	float MacroBlurFocus = 0.05;

	// The depth blur focus at the largest focal distance.
	UPROPERTY(EditDefaultsOnly, Category = "Depth Of Field",
	Meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0", EditCondition = "bDynamicDOFEnabled"))
	float LongShotBlurFocus = 0.365;

	// The depth blur Amount at the smallest focal distance.
	UPROPERTY(EditDefaultsOnly, Category = "Depth Of Field",
	Meta = (ClampMin = "0.0", ClampMax = "4.0", UIMin = "0.0", UIMax = "4.0", EditCondition = "bDynamicDOFEnabled"))
	float MacroBlurAmount = 1.4;

	// The depth blur Amount at the largest focal distance.
	UPROPERTY(EditDefaultsOnly, Category = "Depth Of Field",
	Meta = (ClampMin = "0.0", ClampMax = "4.0", UIMin = "0.0", UIMax = "4.0", EditCondition = "bDynamicDOFEnabled"))
	float LongShotBlurAmount = 1.1;

	// The interpolation speed for the dynamic depth of field algorithm
	UPROPERTY(EditDefaultsOnly, Category = "Depth Of Field",
	Meta = (ClampMin = "0.0", ClampMax = "10.0", UIMin = "0.0", UIMax = "10.0", EditCondition = "bDynamicDOFEnabled"))
	float DynamicDofSpeed = 6.75;

	// When enabled, the vignette effect on the camera will be increased when sprinting.
	UPROPERTY(EditDefaultsOnly, Category = "Vignette")
	bool bDynamicVignetteEnabled = false;

	UPROPERTY(EditDefaultsOnly, Category = "Vignette",
	Meta = (ClampMin = "1", UIMin = "1", EditCondition = "bDynamicVignetteEnabled"))
	float DynamicVignetteSpeed = 3.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Vignette",
	Meta = (XAxisName="Speed", YAxisName="Intensity", EditCondition = "bDynamicVignetteEnabled",
		DisplayName = "Intensity over Velocity"))
	FRuntimeFloatCurve DynamicVignetteIntensityCurve {};

	UPROPERTY()
	FBHCameraQuery CameraQuery {};

	FCollisionQueryParams CameraQueryParams {};

	UPROPERTY(Transient)
	FPostProcessSettings BasePostProcessSettings;
};
