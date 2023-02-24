// Copyright 2023 Barrelhouse


#include "ExteriorWindAudioComponent.h"
#include "GameFramework/Actor.h"
#include "MetasoundSource.h"
#include "Components/AudioComponent.h"

// Sets default values for this component's properties
UExteriorWindAudioComponent::UExteriorWindAudioComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
}


// Called when the game starts
void UExteriorWindAudioComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UExteriorWindAudioComponent::InitializeComponent()
{
	TraceVectors = GetTraceVectors(3000, 8);

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

// Called every frame
void UExteriorWindAudioComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if(GetOwner() && (GetOwner()->GetActorLocation() - LastPollLocation).SquaredLength() > 62500)
	{
		LastPollLocation = GetOwner()->GetActorLocation();
		if(GetOwner())
		{
			const TArray<float> PollResults {PollLocation(LastPollLocation)};
			EventOnPoll(PollResults);
		}
	}
}

void UExteriorWindAudioComponent::EventOnPoll_Implementation(const TArray<float>& PollResults)
{
}

TArray<float> UExteriorWindAudioComponent::PollLocation(const FVector& Location)
{
	TArray<float> TraceLengths;
	for (const FVector2D Vector : TraceVectors)
	{
		const FVector TraceStart {GetOwner()->GetActorLocation()};
		const FVector TraceEnd {FVector(Vector.X, Vector.Y, 0) + GetOwner()->GetActorLocation()};

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

TArray<FVector2D> UExteriorWindAudioComponent::GetTraceVectors(const float Radius, const float NumPoints) const
{
	const float AngleIncrement {2.f * PI / NumPoints};
	TArray<FVector2D> Vectors;
	
	for (int i = 0; i < NumPoints; i++)
	{
		const float Angle = i * AngleIncrement;
		const float X = Radius * FMath::Cos(Angle);
		const float Y = Radius * FMath::Sin(Angle);
		Vectors.Add(FVector2D(X, Y)) ;
	}
	return Vectors;
}

// Called when before the object is destroyed.
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

