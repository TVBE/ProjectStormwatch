// Copyright (c) 2022-present Tim Verberne
// This source code is part of the Adaptive Ambience System plugin

#include "AmbiverseParameter.h"

float UAmbiverseParameter::SetParameter(float Value)
{
	ParameterValue = FMath::GetMappedRangeValueClamped(ParameterRange, FVector2D(0, 1), Value);
	return ParameterValue;
}

void UAmbiverseParameter::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	{
		Super::PostEditChangeProperty(PropertyChangedEvent);

		if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UAmbiverseParameter, DefaultValue))
		{
			DefaultValue = FMath::Clamp(DefaultValue, ParameterRange.X, ParameterRange.Y);
		}
	}
}
