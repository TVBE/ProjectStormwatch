// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#include "ExteriorWindAudioComponent.h"

#include "GameFramework/Actor.h"
#include "MetasoundSource.h"
#include "Components/AudioComponent.h"

/** Sets default values for this component's properties. */
UExteriorWindAudioComponent::UExteriorWindAudioComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
}

/** Called when the game starts. */
void UExteriorWindAudioComponent::BeginPlay()
{
	Super::BeginPlay();
}

/** Called when the component is initialized. */
void UExteriorWindAudioComponent::InitializeComponent()
{
	Super::InitializeComponent();
	
	/** Initialize the trace vector arrays. */
	PopulateGeometryTraceVectors(GeometryTraceVectors, WindDirection, CollisionTraceLength, 8, TemporalTraceLength, TemporalTracePitchIncrement, TemporalTracePitchOffset);
	PopulateOcclusionTraceVectors(OcclusionTraceStartVectors, OcclusionTraceEndVectors, WindDirection, CollisionTraceLength, 250);
	
	if (GetOwner())
	{
		AudioComponent = static_cast<UAudioComponent*>(GetOwner()->AddComponentByClass(UAudioComponent::StaticClass(), false, FTransform(), true));
	}
	if (AudioComponent && MetaSoundAsset)
	{
		AudioComponent->SetSound(MetaSoundAsset.LoadSynchronous());
	}

	/** Initialize the geometry query results array at a fixed size. */
	GeometryQueryResults.Init(FHitResult(), 8 * TemporalTraceLength);
}

void UExteriorWindAudioComponent::SetWindDirection(const FRotator& Rotation)
{
	if (WindDirection == Rotation)
	{
		return;
	}
	WindDirection = Rotation;
	PopulateGeometryTraceVectors(GeometryTraceVectors, WindDirection, CollisionTraceLength, 8, TemporalTraceLength, TemporalTracePitchIncrement, TemporalTracePitchOffset);
	PopulateOcclusionTraceVectors(OcclusionTraceStartVectors, OcclusionTraceEndVectors, WindDirection, CollisionTraceLength, 250);
	EventOnWindDirectionChanged(Rotation);
}

/** Called every frame. */
void UExteriorWindAudioComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateGeometryQuery();
	
	if (GetOwner() && (GetOwner()->GetActorLocation() - LastPollLocation).SquaredLength() > 62500)
	{
		LastPollLocation = GetOwner()->GetActorLocation();
		if (GetOwner())
		{
			if (IsQueryingGeometry)
			{
				IsGeometryQueryQueued = true;
			}
			else
			{
				BeginTemporalGeometryQuery();
			}
			
			const TArray<float> OcclusionTraceResults {DoOcclusionCollisionQuery(LastPollLocation)};
			EventOnOcclusionPoll(OcclusionTraceResults);
		}
	}
}

TArray<float> UExteriorWindAudioComponent::DoTerrainCollisionQuery(const FVector& Location)
{
	TArray<float> TraceLengths;
	for (int i {0}; i < GeometryTraceVectors.Num(); i++)
	{
		const FVector TraceStart {GetOwner()->GetActorLocation()};
		const FVector TraceEnd {GetOwner()->GetActorLocation() + GeometryTraceVectors[i]}; 

		FHitResult HitResult;
		FCollisionQueryParams Params {FCollisionQueryParams::DefaultQueryParam};
		Params.bTraceComplex = false;
		Params.bReturnPhysicalMaterial = false;

		if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, Params))
		{
			const float TraceLength {static_cast<float>((HitResult.ImpactPoint - TraceStart).Size())};
			TraceLengths.Add(TraceLength);
		}
		else
		{
			const float TraceLength {static_cast<float>((TraceEnd - TraceStart).Size())};
			TraceLengths.Add(TraceLength);
		}
	}
	return TraceLengths;
}

TArray<float> UExteriorWindAudioComponent::DoOcclusionCollisionQuery(const FVector& Location)
{
	TArray<float> TraceLengths;
	for (int i {0}; i < OcclusionTraceStartVectors.Num(); i++)
	{
		const FVector TraceStart {GetOwner()->GetActorLocation() + OcclusionTraceStartVectors[i]};
		const FVector TraceEnd {GetOwner()->GetActorLocation() + OcclusionTraceEndVectors[i]};

		FHitResult HitResult;
		FCollisionQueryParams Params {FCollisionQueryParams::DefaultQueryParam};
		Params.bTraceComplex = false;
		Params.bReturnPhysicalMaterial = false;

		if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, Params))
		{
			const float TraceLength {static_cast<float>((HitResult.ImpactPoint - TraceStart).Size())};
			TraceLengths.Add(TraceLength);
		}
		else
		{
			const float TraceLength {static_cast<float>((TraceEnd - TraceStart).Size())};
			TraceLengths.Add(TraceLength);
		}
	}
	return TraceLengths;
}

float UExteriorWindAudioComponent::GetAverageOfFloatArray(const TArray<float>& Array) const
{
	float Sum {0.0f};
	for(const float Value : Array)
	{
		Sum += Value;
	}
	Sum /= Array.Num();
	return Sum;
}

float UExteriorWindAudioComponent::GetAverageTraceLengthInCardinalDirection(const ETraceCardinalDirection Direction)
{
	TArray<float> TraceLengths;

	int StartIndex {0};
	switch (Direction)
	{
	case ETraceCardinalDirection::North:
		StartIndex = 0;
		break;
	case ETraceCardinalDirection::East:
		StartIndex = 16;
		break;
	case ETraceCardinalDirection::South:
		StartIndex = 32;
		break;
	case ETraceCardinalDirection::West:
		StartIndex = 48;
		break;
	}

	for (int32 i {StartIndex}; i < StartIndex + 16; ++i)
	{
		const FHitResult& HitResult {GeometryQueryResults[i]};
		if (HitResult.bBlockingHit)
		{
			TraceLengths.Add(GeometryQueryResults[i].Distance);
		}
		else
		{
			TraceLengths.Add(CollisionTraceLength);
		}
	}

	return GetAverageOfFloatArray(TraceLengths);
}

void UExteriorWindAudioComponent::BeginTemporalGeometryQuery()
{
	ResetGeometryQueryResults();
	IsQueryingGeometry = true;
	TemporalQueryOrigin = GetOwner()->GetActorLocation();
}

void UExteriorWindAudioComponent::ResetGeometryQueryResults()
{
	for (FHitResult& HitResult : GeometryQueryResults)
	{
		HitResult.Reset();
	}
}

void UExteriorWindAudioComponent::UpdateGeometryQuery()
{
	if (!IsQueryingGeometry) { return; }

	if (CurrentTemporalTraceFrame >= TemporalTraceLength - 1)
	{
		CurrentTemporalTraceFrame = 0;
		if (IsGeometryQueryQueued)
		{
			BeginTemporalGeometryQuery();
		}
		else
		{
			IsQueryingGeometry = false;
		}
		EventOnGeometryQueryFinished(GeometryQueryResults);
		return;
	}

	const int StartIndex {CurrentTemporalTraceFrame * 8};
	const int EndIndex {StartIndex + 8};

	for (int i {StartIndex}; i <= EndIndex; ++i)
	{
		const FVector TraceStart {TemporalQueryOrigin};
		const FVector TraceEnd {TemporalQueryOrigin + GeometryTraceVectors[i]}; 

		FHitResult& HitResult {GeometryQueryResults[i]};
		FCollisionQueryParams Params {FCollisionQueryParams::DefaultQueryParam};
		Params.bTraceComplex = false;
		Params.bReturnPhysicalMaterial = false;
		GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, Params);

#if WITH_EDITOR
		if (IsTraceVisEnabled)
		{
			DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);
		}
#endif
	}

	CurrentTemporalTraceFrame++;
}

/** Populates the terrain trace array. */
void UExteriorWindAudioComponent::PopulateGeometryTraceVectors(TArray<FVector>& Array, const FRotator& Rotation,
	const float Radius, const float NumPoints, const uint8 TemporalFrames, const float PitchIncrement, const float PitchOffset)
{
	const float AngleIncrement {2.f * PI / NumPoints};

	TArray<FVector> SingleFrameTraceVectors;
	
	for (int i {0}; i < NumPoints; i++)
	{
		const float Angle {i * AngleIncrement};
		const float X {Radius * FMath::Cos(Angle)};
		const float Y {Radius * FMath::Sin(Angle)};
		SingleFrameTraceVectors.Add(FVector(X, Y, 0)) ;
	}
	
	float TargetPitchAngle {0.0f};
	
	
	for (int i {0}; i < TemporalFrames; i++)
	{
		constexpr float YawIncrement = 22.5f;
		FRotator YawRotation(0, i * YawIncrement, 0);
		FQuat YawQuat(YawRotation);
		
		for (const FVector& TraceVector : SingleFrameTraceVectors)
		{
			const FVector RotatedVector = YawQuat.RotateVector(TraceVector);
			
			const double Azimuth {FMath::Atan2(RotatedVector.Y, RotatedVector.X)};
			double Elevation {FMath::Acos(RotatedVector.Z / Radius)};

			Elevation += FMath::DegreesToRadians(-TargetPitchAngle + PitchOffset);

			const double X {Radius * FMath::Sin(Elevation) * FMath::Cos(Azimuth)};
			const double Y {Radius * FMath::Sin(Elevation) * FMath::Sin(Azimuth)};
			const double Z {Radius * FMath::Cos(Elevation)};

			Array.Add(FVector(X, Y, Z));
		}
		TargetPitchAngle += PitchIncrement;
	}
	SortTraceVectorsByYaw(Array);
}

void UExteriorWindAudioComponent::SortTraceVectorsByYaw(TArray<FVector>& TraceVectors)
{
	TArray<FVector> NorthVectors;
	TArray<FVector> EastVectors;
	TArray<FVector> SouthVectors;
	TArray<FVector> WestVectors;

	for (const FVector& TraceVector : TraceVectors)
	{
		float Yaw {static_cast<float>(FMath::RadiansToDegrees(FMath::Atan2(TraceVector.Y, TraceVector.X)))};

		if (Yaw < 0.0f)
		{
			Yaw += 360.0f;
		}

		if (Yaw >= 315.0f || Yaw < 45.0f)
		{
			NorthVectors.Add(TraceVector);
		}
		else if (Yaw >= 45.0f && Yaw < 135.0f)
		{
			EastVectors.Add(TraceVector);
		}
		else if (Yaw >= 135.0f && Yaw < 225.0f)
		{
			SouthVectors.Add(TraceVector);
		}
		else 
		{
			WestVectors.Add(TraceVector);
		}
	}
	
	TraceVectors.Empty();
	TraceVectors.Append(NorthVectors);
	TraceVectors.Append(EastVectors);
	TraceVectors.Append(SouthVectors);
	TraceVectors.Append(WestVectors);
}


/** Populates the occlusion trace arrays. */
void UExteriorWindAudioComponent::PopulateOcclusionTraceVectors(TArray<FVector>& ArrayA, TArray<FVector>& ArrayB,
	const FRotator& Rotation, const float TraceLength, const float Spacing)
{
	/** Empty both arrays. */
	ArrayA.Empty();
	ArrayB.Empty();
	
	/** Create an array of occlusion trace offsets. */
	TArray<FVector2D> Offsets {FVector2D()};
	Offsets.Add(FVector2D(-1, -1));
	Offsets.Add(FVector2D(0, -1));
	Offsets.Add(FVector2D(1, -1));
	Offsets.Add(FVector2D(-1, 0));
	Offsets.Add(FVector2D(0, 0));
	Offsets.Add(FVector2D(1, 0));
	Offsets.Add(FVector2D(-1, 1));
	Offsets.Add(FVector2D(0, 1));
	Offsets.Add(FVector2D(1, 1));

	/** Populate Array A. */
	for (const FVector2D Offset : Offsets)
	{
		const FVector Vector {Rotation.RotateVector(FVector(0, Offset.X * Spacing,Offset.Y * Spacing))};
		ArrayA.Add(Vector);
	}

	/** Populate Array B. */
	for (const FVector TraceStart : ArrayA)
	{
		const FVector Vector {TraceStart - Rotation.RotateVector(FVector(TraceLength, 0, 0))};
		ArrayB.Add(Vector);
	}
}

/** Called when before the object is destroyed. */
void UExteriorWindAudioComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AudioComponent)
	{
		if (AudioComponent->IsPlaying())
		{
			AudioComponent->Stop();
		}
		AudioComponent->DestroyComponent();
	}
	Super::EndPlay(EndPlayReason);
}

TArray<FHitResult> UExteriorWindAudioComponent::GetGeometryQueryResults() const
{
	if (!IsQueryingGeometry)
	{
		return GeometryQueryResults;
	}
	TArray<FHitResult> EmptyArray;
	return EmptyArray;
}

void UExteriorWindAudioComponent::EventOnOcclusionPoll_Implementation(const TArray<float>& OcclusionTraceResults)
{
}

void UExteriorWindAudioComponent::EventOnGeometryQueryFinished_Implementation(
	const TArray<FHitResult>& GeometryTraceResults)
{
}

void UExteriorWindAudioComponent::EventOnWindDirectionChanged_Implementation(const FRotator& Rotation)
{
}


