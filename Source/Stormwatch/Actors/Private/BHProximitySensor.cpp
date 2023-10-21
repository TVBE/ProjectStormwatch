// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHProximitySensor.h"

#include "BHNightstalker.h"
#include "BHPlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"

DEFINE_LOG_CATEGORY_CLASS(ABHProximitySensor, LogSensor)

ABHProximitySensor::ABHProximitySensor()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	DetectionArea = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Detection Area"));
	DetectionArea->bEditableWhenInherited = true;
	DetectionArea->SetupAttachment(this->RootComponent);
	DetectionArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DetectionArea->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	DetectionArea->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	DetectionArea->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void ABHProximitySensor::BeginPlay()
{
	Super::BeginPlay();

	if (DetectionArea)
	{
		DetectionArea->OnComponentBeginOverlap.AddDynamic(this, &ABHProximitySensor::OnOverlapBegin);
		DetectionArea->OnComponentEndOverlap.AddDynamic(this, &ABHProximitySensor::OnOverlapEnd);
	}
}

void ABHProximitySensor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(ABHPlayerCharacter::StaticClass()) && IgnoreParameters.Contains(EBHProximitySensorIgnoreParameter::Player))
	{
		return;
	}
	if (OtherActor->IsA(ANightstalker::StaticClass()) && IgnoreParameters.Contains(EBHProximitySensorIgnoreParameter::Nightstalker))
	{
		return;
	}
	OverlappingActors.AddUnique(OtherActor);

	Poll();
	GetWorldTimerManager().SetTimer(PollTimerHandle, this, &ABHProximitySensor::Poll, PollInterval, true);
}

void ABHProximitySensor::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	OverlappingActors.Remove(OtherActor);

	if (OverlappingActors.IsEmpty())
	{
		if (const UWorld* World = GetWorld())
		{
			if (World->GetTimerManager().IsTimerActive(PollTimerHandle))
			{
				World->GetTimerManager().ClearTimer(PollTimerHandle);
			}
		}
	}

	if (IsAlerted && !IsTriggered)
	{
		SetState(EBHSensorState::Alerted);
		StartCooldown();
	}
}

/** Called at regular intervals to check for overlapping actors and determine the nearest pawn. */
void ABHProximitySensor::Poll()
{
	IsActorDetected = false;

	if (IsBroken) =  return; 
	
	if (!OverlappingActors.IsEmpty())
	{
		for (const AActor* Actor : OverlappingActors)
		{
			if (!IsActorOccluded(Actor))
			{
				IsActorDetected = true;
				break;
			}
		}
	}
	
	if (IsActorDetected)
	{
		if (IsTriggered)
		{
			return;
		}

		SetState(EBHSensorState::Detecting);

		if (const UWorld* World = GetWorld())
		{
			if (World->GetTimerManager().IsTimerActive(CooldownTimerHandle))
			{
				World->GetTimerManager().ClearTimer(CooldownTimerHandle);
			}
		}
		
		DetectionLevel += PollInterval;
		if (!IsAlerted)
		{
			IsAlerted = true;
		}
		
		if (DetectionLevel >= DetectionThreshold)
		{
			IsTriggered = true;
			
			if (IsManualResetRequired)
			{
				if (const UWorld* World = GetWorld())
				{
					if (World->GetTimerManager().IsTimerActive(PollTimerHandle))
					{
						World->GetTimerManager().ClearTimer(PollTimerHandle);
					}
				}
			}

			SetState(EBHSensorState::Triggered);
		}
	}
	else
	{
		if (IsAlerted)
		{
			SetState(EBHSensorState::Alerted);
			
			if (const UWorld* World = GetWorld())
			{
				if (World->GetTimerManager().IsTimerActive(CooldownTimerHandle))
				{
					return;
				}
				StartCooldown();
			}
		}
	}
}

/** Determines if the given actor is occluded by another object using a line trace. */
bool ABHProximitySensor::IsActorOccluded(const AActor* Actor) const
{
	FVector StartLocation = GetActorLocation() - FVector(0, 0, 10);
	FVector ActorLocation = Actor->GetActorLocation();
	TArray<FVector> EndLocations
	{
		ActorLocation,
		ActorLocation + FVector(0.f, 0.f, 50.f),
		ActorLocation - FVector(0.f, 0.f, 50.f)
	};

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	CollisionParams.AddIgnoredActor(Actor);

	uint8 BlockedTraces = 0;

	for (const FVector& EndLocation : EndLocations)
	{
		FHitResult HitResult;

		if (bool IsHit {GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, CollisionParams)})
		{
			AActor* HitActor = HitResult.GetActor();

			if (HitActor != Actor)
			{
				++BlockedTraces;
			}
		}
	}

	return BlockedTraces == EndLocations.Num();
}

void ABHProximitySensor::StartCooldown()
{
	if (IsBroken) { return; }
	
	IsCooldownActive = true;
	
	if (const UWorld* World = GetWorld())
	{
		if (World->GetTimerManager().TimerExists(CooldownTimerHandle))
		{
			World->GetTimerManager().ClearTimer(CooldownTimerHandle);
			
		}

		World->GetTimerManager().SetTimer(CooldownTimerHandle, this, &ABHProximitySensor::HandleCooldownFinished, CooldownTime, false);
	}
}

void ABHProximitySensor::StopCooldown()
{
	if (const UWorld* World = GetWorld())
	{
		if (World->GetTimerManager().IsTimerActive(CooldownTimerHandle))
		{
			World->GetTimerManager().ClearTimer(CooldownTimerHandle);
			IsCooldownActive = false;
		}
	}
}

void ABHProximitySensor::SetState(const EBHSensorState NewState)
{
	if (SensorState != NewState)
	{
		SensorState = NewState;
		
		OnStateChanged.Broadcast(NewState);
	}
}

void ABHProximitySensor::HandleCooldownFinished()
{
	DetectionLevel = 0.0f;
	
	IsCooldownActive = false;
	
	IsAlerted = false;
	
	IsTriggered = false;

	if (IsBroken) { return; }
	
	SetState(EBHSensorState::Idle);
}

void ABHProximitySensor::ResetSensor()
{
	IsAlerted = false;
	IsTriggered = false;
	DetectionLevel = 0.0f;

	if (const UWorld* World = GetWorld())
	{
		if (World->GetTimerManager().IsTimerActive(CooldownTimerHandle))
		{
			World->GetTimerManager().ClearTimer(CooldownTimerHandle);
			IsCooldownActive = false;
		}
	}
	else { return;}

	GetWorldTimerManager().SetTimer(PollTimerHandle, this, &ABHProximitySensor::Poll, PollInterval, true);
	
	SetState(EBHSensorState::Idle);
}

void ABHProximitySensor::ActivateSensor()
{
	if (!IsSensorActive)
	{
		if (DetectionArea)
		{
			DetectionArea->OnComponentBeginOverlap.AddDynamic(this, &ABHProximitySensor::OnOverlapBegin);
			DetectionArea->OnComponentEndOverlap.AddDynamic(this, &ABHProximitySensor::OnOverlapEnd);
		}

		DetectionArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		
		IsSensorActive = true;

		OnActivation.Broadcast();
		SetState(EBHSensorState::Idle);

		Poll();
	}
}

void ABHProximitySensor::DeactivateSensor()
{
	if (IsSensorActive)
	{
		IsSensorActive = false;
		
		StopCooldown();
		
		if (DetectionArea)
		{
			DetectionArea->OnComponentBeginOverlap.RemoveDynamic(this, &ABHProximitySensor::OnOverlapBegin);
			DetectionArea->OnComponentEndOverlap.RemoveDynamic(this, &ABHProximitySensor::OnOverlapEnd);
		}

		if (const UWorld* World = GetWorld())
		{
			if (World->GetTimerManager().IsTimerActive(PollTimerHandle))
			{
				World->GetTimerManager().ClearTimer(PollTimerHandle);
			}
		}
		
		IsAlerted = false;
		IsTriggered = false;
		IsActorDetected = false;
		
		SetState(EBHSensorState::Disabled);

		DetectionArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ABHProximitySensor::BreakSensor()
{
	if (!IsBroken)
	{
		IsBroken = true;

		StopCooldown();
		
		if (DetectionArea)
		{
			DetectionArea->OnComponentBeginOverlap.RemoveDynamic(this, &ABHProximitySensor::OnOverlapBegin);
			DetectionArea->OnComponentEndOverlap.RemoveDynamic(this, &ABHProximitySensor::OnOverlapEnd);
		}

		if (const UWorld* World = GetWorld())
		{
			if (World->GetTimerManager().IsTimerActive(PollTimerHandle))
			{
				World->GetTimerManager().ClearTimer(PollTimerHandle);
			}
		}
		
		IsAlerted = false;
		IsTriggered = false;
		IsActorDetected = false;
		
		SetState(EBHSensorState::Broken);

		DetectionArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

#if WITH_EDITOR
void ABHProximitySensor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ABHProximitySensor, IgnoreParameters))
	{
		TSet<EBHProximitySensorIgnoreParameter> UniqueIgnoreParameters;

		for (int Index = 0; Index < IgnoreParameters.Num(); ++Index)
		{
			if (!UniqueIgnoreParameters.Contains(IgnoreParameters[Index]))
			{
				UniqueIgnoreParameters.Add(IgnoreParameters[Index]);
			}
			else
			{
				IgnoreParameters.RemoveAt(Index);
				--Index;
				UE_LOG(LogSensor, Warning, TEXT("Duplicate ignore parameters have been detected and removed."));
			}
		}
	}
}
#endif



