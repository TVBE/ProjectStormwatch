// Copyright (c) 2023-present Tim Verberne. All rights reserved.

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

bool AAmbiverseSoundSource::Initialize(FAmbiverseSoundSourceData& Data, UAmbiverseElementInstance* ElementInstance)
{
	UE_LOG(LogTemp, Warning, TEXT("Initializing Sound Source"))
	
	if (!Data.Sound)
	{
		UE_LOG(LogAmbiverseSoundSource, Warning, TEXT("Sound is nullptr."));
		return false;
	}
	SoundSourceData = Data;
	
	SetSound(SoundSourceData.Sound);
	SetActorTransform(SoundSourceData.Transform);
	
	if(AudioComponent)
	{
		AudioComponent->Play();
		BeginPlayback();
	}
	else
	{
		UE_LOG(LogAmbiverseSoundSource, Warning, TEXT("AudioComponent is nullptr."));
	}

	if (ElementInstance)
	{
		AssociatedElement = ElementInstance;
	}
	else
	{
		UE_LOG(LogAmbiverseSoundSource, Warning, TEXT("SoundSource has no associated element."))
	}

	return true;
}

bool AAmbiverseSoundSource::Deinitialize()
{
	AssociatedElement = nullptr;
	SoundSourceData = {};

	return true;
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
#if !UE_BUILD_SHIPPING
		ActiveTime = 0;
#endif
	
		EndPlayback();
		OnFinishedPlayback.Execute(this);
}


void AAmbiverseSoundSource::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

/*#if !UE_BUILD_SHIPPING
	if (AudioComponent->IsPlaying())
	{
		ActiveTime += DeltaTime;
	}
	
	if(IsDebugVisualisationEnabled)
	{
		const APlayerCameraManager* CameraManager {UGameplayStatics::GetPlayerCameraManager(this, 0)};
		const uint32 DistanceToCamera {static_cast<uint32>((CameraManager->GetCameraLocation() - GetActorLocation()).Size())};

		const float TimeInSeconds {ActiveTime / 1000.0f};
		
		const FString DebugMessage {FString::Printf(TEXT("%s [%i] [%f.2f s]"), *, DistanceToCamera, TimeInSeconds)};
		const FVector Location {GetActorLocation()};
		
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, DebugMessage);
		
		// Draw debug string in world space
		DrawDebugString(GetWorld(), Location, DebugMessage, nullptr, FColor::Red, 0.0f, true);
	}

#endif*/
}

