// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#include "AmbiverseSubsystem.h"

#include "AmbiverseDistributor.h"
#include "AmbiverseDistributorManager.h"
#include "AmbiverseElement.h"
#include "AmbiverseLayer.h"
#include "AmbiverseLayerManager.h"
#include "AmbiverseParameterManager.h"
#include "AmbiverseSoundSourceData.h"
#include "AmbiverseSoundSourceManager.h"

DEFINE_LOG_CATEGORY_CLASS(UAmbiverseSubsystem, LogAmbiverseSubsystem);

UAmbiverseSubsystem::UAmbiverseSubsystem()
{
	
}

void UAmbiverseSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	LayerManager = NewObject<UAmbiverseLayerManager>(this);
	if (LayerManager) { LayerManager->Initialize(this); }
	
	ParameterManager = NewObject<UAmbiverseParameterManager>(this);
	if (ParameterManager) { ParameterManager->Initialize(this); }

	SoundSourceManager = NewObject<UAmbiverseSoundSourceManager>(this);
	if (SoundSourceManager) { SoundSourceManager->Initialize(this); }

	DistributorManager = NewObject<UAmbiverseDistributorManager>(this);
	if (DistributorManager) { DistributorManager->Initialize(this); }

#if !UE_BUILD_SHIPPING
	SoundSourceVisualisationConsoleCommand = MakeUnique<FAutoConsoleCommand>(
		TEXT("av.EnableSoundSourceVisualisation"),
		TEXT("Enable or disable sound source visualisation."),
		FConsoleCommandWithArgsDelegate::CreateLambda([this](const TArray<FString>& Args)
		{
			if (Args.Num() > 0)
			{
				SetSoundSourceVisualisationEnabled(FCString::Atoi(*Args[0]) != 0);
			}
		})
	);
#endif
}

void UAmbiverseSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	
	UE_LOG(LogAmbiverseSubsystem, Log, TEXT("Adaptive Ambience System initialized successfully."))
}

void UAmbiverseSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (LayerManager)
	{
		LayerManager->Tick(DeltaTime);
	}
}

void UAmbiverseSubsystem::ProcessProceduralElement(UAmbiverseLayer* Layer, FAmbiverseProceduralElement& ProceduralElement)
{
	if (!Layer) { return; }
	
	ProceduralElement.ReferenceTime = FMath::RandRange(ProceduralElement.IntervalRange.X, ProceduralElement.IntervalRange.Y);

	float DensityModifier {1.0f};
	float VolumeModifier {1.0f};

	ParameterManager->GetScalarsForElement(DensityModifier, VolumeModifier, Layer, ProceduralElement);

	ProceduralElement.Time = ProceduralElement.ReferenceTime * DensityModifier;

	/** We try to get the location of the listener here.*/
	FVector CameraLocation;
	bool IsCameraValid {false};
		
	if (APlayerController* PlayerController {GetWorld()->GetFirstPlayerController()})
	{
		if (const APlayerCameraManager* CameraManager {PlayerController->PlayerCameraManager})
		{
			CameraLocation = CameraManager->GetCameraLocation();
			IsCameraValid = true;
		}
	}

	if (!IsCameraValid) { return; }

	/** Prepare the sound source data. */
	FAmbiverseSoundSourceData SoundSourceData {FAmbiverseSoundSourceData()};
	
	SoundSourceData.Sound = UAmbiverseElement::GetSoundFromMap(ProceduralElement.Element->Sounds);
	SoundSourceData.Volume = ProceduralElement.Element->Volume;
	SoundSourceData.Name = FName(ProceduralElement.Element->GetName());
	SoundSourceData.Layer = Layer;

	if (const TSubclassOf<UAmbiverseDistributor> DistributorClass {ProceduralElement.Element->DistributorClass})
	{
		if (UAmbiverseDistributor* Distributor {DistributorManager->GetDistributorByClass(DistributorClass)})
		{
			FTransform Transform {};
			if (Distributor->ExecuteDistribution(this, Transform, CameraLocation, ProceduralElement.Element))
			{
				SoundSourceData.Transform = Transform;
			}
		}
	}
	else
	{
		SoundSourceData.Transform = FAmbiverseSoundDistributionData::GetSoundTransform(ProceduralElement.Element->DistributionData, CameraLocation);
	}
	
	DrawDebugSphere(GetWorld(), SoundSourceData.Transform.GetLocation(), 100.0f, 12, FColor::Red, false, 10.0f);
	
	SoundSourceManager->InitiateSoundSource(SoundSourceData);
	UE_LOG(LogAmbiverseSubsystem, Warning, TEXT("Update"))
}

float UAmbiverseSubsystem::GetSoundInterval(const UAmbiverseLayer* Layer, const FAmbiverseProceduralElement& ProceduralElement)
{
	if (!Layer) { return -1.0f; }
	float Interval {static_cast<float>(FMath::RandRange(ProceduralElement.IntervalRange.X, ProceduralElement.IntervalRange.Y))};
	Interval /= Layer->LayerDensity;
	return Interval;
}

float UAmbiverseSubsystem::GetSoundVolume(const UAmbiverseLayer* Layer, const FAmbiverseProceduralElement& ProceduralElement)
{
	if (!Layer) { return -1.0f; }
	float Volume {ProceduralElement.Element->Volume};
	Volume *= Layer->LayerVolume;
	return Volume;
}

void UAmbiverseSubsystem::HandleParameterChanged()
{
	if (!LayerManager || LayerManager->GetLayerRegistry().IsEmpty()) { return; }

	UE_LOG(LogTemp, Warning, TEXT("Parameter Changed!"))

	TArray<UAmbiverseLayer*> Layers {LayerManager->GetLayerRegistry()};
	for (UAmbiverseLayer* Layer : Layers)
	{
		for (FAmbiverseProceduralElement Entry : Layer->ProceduralElements)
		{
			float DensityScalar {1.0f};
			float VolumeScalar {1.0f};
			
			ParameterManager->GetScalarsForElement(DensityScalar, VolumeScalar, Layer, Entry);

			Entry.Time = Entry.ReferenceTime * DensityScalar;
		}
	}
}

#if !UE_BUILD_SHIPPING
void UAmbiverseSubsystem::SetSoundSourceVisualisationEnabled(bool IsEnabled)
{
	if (SoundSourceManager)
	{
		SoundSourceManager->SetSoundSourceVisualisationEnabled(IsEnabled);
	}
}
#endif

void UAmbiverseSubsystem::Deinitialize()
{
	if (LayerManager)
	{
		LayerManager->Deinitialize(this);
		LayerManager = nullptr;
	}
	if (ParameterManager)
	{
		ParameterManager->Deinitialize(this);
		SoundSourceManager = nullptr;
	}
	if (SoundSourceManager)
	{
		SoundSourceManager->Deinitialize(this);
		SoundSourceManager = nullptr;
	}
	if (DistributorManager)
	{
		DistributorManager->Deinitialize(this);
		DistributorManager = nullptr;
	}
	
	Super::Deinitialize();
}


