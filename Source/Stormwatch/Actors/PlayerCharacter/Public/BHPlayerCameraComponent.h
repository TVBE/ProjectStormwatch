// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "Camera/CameraComponent.h"
#include "BHPlayerCameraComponent.generated.h"

USTRUCT(BlueprintType)
struct FBHCameraTraceData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly)
	FHitResult HitResult;

	UPROPERTY(BlueprintReadOnly)
	float TraceDistance = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	bool bValid = false;
	
	void Reset();
};

UCLASS(NotBlueprintable, BlueprintType, ClassGroup = "BHPlayerCharacter")
class STORMWATCH_API UBHPlayerCameraComponent : public UCameraComponent
{
	GENERATED_BODY()
	
public:
	UBHPlayerCameraComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintGetter)
	const FBHCameraTraceData& GetCameraTraceData();
	const FBHCameraTraceData& GetCameraTraceData() const { return TraceData; }

private:
	void DoCameraTrace();

	UPROPERTY(BlueprintGetter = GetCameraTraceData, Category = "Camera")
	FBHCameraTraceData TraceData {};
};
