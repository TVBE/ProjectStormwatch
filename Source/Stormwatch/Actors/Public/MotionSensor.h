// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ProximitySensor.h"
#include "MotionSensor.generated.h"

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "Sensors", Meta = (DisplayName = "Motion Sensor"))
class AMotionSensor : public AProximitySensor
{
	GENERATED_BODY()

protected:
	/** Minimum velocity required to trigger the motion sensor. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float VelocityThreshold = 50.0f;
	
public:
	AMotionSensor();

	virtual void PostInitProperties() override;

protected:
	virtual void Poll() override;

private:
	bool IsActorMoving(const AActor* Actor) const;
};
