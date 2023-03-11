// Copyright 2023 Barrelhouse

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
	/** Initialize the trace vector arrays. */
	PopulateTerrainTraceVectors(TerrainTraceEndVectors, WindDirection, CollisionTraceLength, 8);
	PopulateOcclusionTraceVectors(OcclusionTraceStartVectors, OcclusionTraceEndVectors, WindDirection, CollisionTraceLength, 250);
	
	if(GetOwner())
	{
		AudioComponent = static_cast<UAudioComponent*>(GetOwner()->AddComponentByClass(UAudioComponent::StaticClass(), false, FTransform(), true));
	}
	if(AudioComponent && MetaSoundAsset)
	{
		AudioComponent->SetSound(MetaSoundAsset.LoadSynchronous());
	}
	
	Super::InitializeComponent();
}

void UExteriorWindAudioComponent::SetWindDirection(const FRotator& Rotation)
{
	if(WindDirection == Rotation)
	{
		return;
	}
	WindDirection = Rotation;
	PopulateTerrainTraceVectors(TerrainTraceEndVectors, WindDirection, CollisionTraceLength, 8);
	PopulateOcclusionTraceVectors(OcclusionTraceStartVectors, OcclusionTraceEndVectors, WindDirection, CollisionTraceLength, 250);
	EventOnWindDirectionChanged(Rotation);
}

/** Called every frame. */
void UExteriorWindAudioComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if(GetOwner() && (GetOwner()->GetActorLocation() - LastPollLocation).SquaredLength() > 62500)
	{
		LastPollLocation = GetOwner()->GetActorLocation();
		if(GetOwner())
		{
			const TArray<float> TerrainTraceResults {DoTerrainCollisionQuery(LastPollLocation)};
			const TArray<float> OcclusionTraceResults {DoOcclusionCollisionQuery(LastPollLocation)};
			EventOnPoll(TerrainTraceResults, OcclusionTraceResults);
		}
	}
}

TArray<float> UExteriorWindAudioComponent::DoTerrainCollisionQuery(const FVector& Location)
{
	TArray<float> TraceLengths;
	for (int i {0}; i < TerrainTraceEndVectors.Num(); i++)
	{
		const FVector TraceStart {GetOwner()->GetActorLocation()};
		const FVector TraceEnd {GetOwner()->GetActorLocation() + TerrainTraceEndVectors[i]}; 

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

/** Populates the terrain trace array. */
void UExteriorWindAudioComponent::PopulateTerrainTraceVectors(TArray<FVector>& Array, const FRotator& Rotation, const float Radius, const float NumPoints)
{
	const float AngleIncrement {2.f * PI / NumPoints};
	
	for (int i = 0; i < NumPoints; i++)
	{
		const float Angle {i * AngleIncrement};
		const float X {Radius * FMath::Cos(Angle)};
		const float Y {Radius * FMath::Sin(Angle)};
		Array.Add(FVector(X, Y, 0)) ;
	}
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
	if(AudioComponent)
	{
		if(AudioComponent->IsPlaying())
		{
			AudioComponent->Stop();
		}
		AudioComponent->DestroyComponent();
	}
	Super::EndPlay(EndPlayReason);
}

void UExteriorWindAudioComponent::EventOnPoll_Implementation(const TArray<float>& TerrainTraceResults, const TArray<float>& OcclusionTraceResults)
{
}

void UExteriorWindAudioComponent::EventOnWindDirectionChanged_Implementation(const FRotator& Rotation)
{
}
