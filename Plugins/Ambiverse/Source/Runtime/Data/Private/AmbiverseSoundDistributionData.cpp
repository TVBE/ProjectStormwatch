// Copyright (c) 2022-present Tim Verberne
// This source code is part of the Adaptive Ambience System plugin

#include "AmbiverseSoundDistributionData.h"

FTransform FAmbiverseSoundDistributionData::GetSoundTransform(const FAmbiverseSoundDistributionData& DistributionData,
	const FVector& ListenerLocation)
{
	FTransform Transform;

	const double X {FMath::RandRange(DistributionData.HorizontalRange.X, DistributionData.HorizontalRange.Y)};
	const double Y {FMath::RandRange(DistributionData.HorizontalRange.X, DistributionData.HorizontalRange.Y)};
	
	double Z {FMath::RandRange(DistributionData.VerticalRange * 0.5, DistributionData.VerticalRange * -0.5)};
	Z += DistributionData.VerticalOffset;

	const FVector Location {FVector(X, Y, Z) + ListenerLocation};

	Transform.SetLocation(Location);
	return Transform;
}
