// Copyright (c) 2022-present Tim Verberne
// This source code is part of the Adaptive Ambience System plugin

#include "AmbiverseSoundSource.h"
#include "AmbiverseSoundSourceManager.h"

#if !UE_BUILD_SHIPPING
#include "Kismet/GameplayStatics.h"
#endif 

DEFINE_LOG_CATEGORY_CLASS(AAmbiverseSoundSource, LogAmbiverseSoundSource);

AAmbiverseSoundSource::AAmbiverseSoundSource()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio Component"));
	if (AudioComponent)
	{
		RootComponent = AudioComponent;
	}
}

void AAmbiverseSoundSource::Initialize(UAmbiverseSoundSourceManager* Manager,
	FAmbiverseSoundSourceData& Data)
{
	if (!Manager)
	{
		return;
	}
	SoundSourceManager = Manager;

	if (!Data.Sound)
	{
		return;
	}
	SoundSourceData = Data;
	
	SetSound(SoundSourceData.Sound);
	SetActorTransform(SoundSourceData.Transform);
	SetVolume(SoundSourceData.Volume);
	SoundSourceName = SoundSourceData.Name;
	AmbiverseLayer = SoundSourceData.Layer;

	if(AudioComponent)
	{
		AudioComponent->Play();
	}
}

void AAmbiverseSoundSource::BeginPlay()
{
	Super::BeginPlay();

	if(AudioComponent)
	{
		AudioComponent->OnAudioFinished.AddDynamic(this, &AAmbiverseSoundSource::HandleOnAudioFinishedPlaying);
	}
}

void AAmbiverseSoundSource::HandleOnAudioFinishedPlaying()
{
	if (SoundSourceManager)
	{
#if !UE_BUILD_SHIPPING
		ActiveTime = 0;
#endif
		
		SoundSourceManager->ReleaseToPool(this);
	}
}


void AAmbiverseSoundSource::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

#if !UE_BUILD_SHIPPING
	if (AudioComponent->IsPlaying())
	{
		ActiveTime += DeltaTime;
	}
	
	if(IsDebugVisualisationEnabled && AmbiverseLayer)
	{
		const APlayerCameraManager* CameraManager {UGameplayStatics::GetPlayerCameraManager(this, 0)};
		const uint32 DistanceToCamera {static_cast<uint32>((CameraManager->GetCameraLocation() - GetActorLocation()).Size())};

		const float TimeInSeconds {ActiveTime / 1000.0f};
		
		const FString DebugMessage {FString::Printf(TEXT("%s [%i] [%f.2f s]"), *SoundSourceName.ToString(), DistanceToCamera, TimeInSeconds)};
		const FVector Location {GetActorLocation()};
		
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, DebugMessage);
		
		// Draw debug string in world space
		DrawDebugString(GetWorld(), Location, DebugMessage, nullptr, FColor::Red, 0.0f, true);
	}

#endif
}

