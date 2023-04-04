// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "ProximitySensor.h"
#include "MotionSensor.generated.h"


UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = Interaction, Meta = (DisplayName = "Motion Sensor"))
class AMotionSensor : public AProximitySensor
{
	GENERATED_BODY()

protected:
	/** Minimum velocity required to trigger the motion sensor. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Motion Sensor", Meta = (DisplayName = "Velocity Threshold"))
	float VelocityThreshold {50.0f};
	
public:
	AMotionSensor();

	virtual void PostInitProperties() override;

protected:
	virtual void Poll() override;

private:
	bool IsActorMoving(const AActor* Actor) const;
};
