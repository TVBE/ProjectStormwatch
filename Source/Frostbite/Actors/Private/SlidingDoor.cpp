// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#include "SlidingDoor.h"
#include "PowerConsumerComponent.h"
#include "Components/BoxComponent.h"

ASlidingDoor::ASlidingDoor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = RootSceneComponent;

	SafetyBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Safety Zone"));
	SafetyBox->SetupAttachment(RootComponent);
	
	SafetyBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SafetyBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	SafetyBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SafetyBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void ASlidingDoor::BeginPlay()
{
	Super::BeginPlay();

	DoorState = StartingState;

	if (RequiresPower)
	{
		PowerConsumerComponent = Cast<UPowerConsumerComponent>
		(AddComponentByClass(UPowerConsumerComponent::StaticClass(), false, FTransform(), true));
		if (PowerConsumerComponent)
		{
			PowerConsumerComponent->PowerSource = PowerSource;
			PowerConsumerComponent->OnPowerStateChanged.AddDynamic(this, &ASlidingDoor::EventOnPowerStateChanged);

			PowerConsumerComponent->RegisterComponent();
			PowerConsumerComponent->InitializeComponent();
		}
	}
}

void ASlidingDoor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (DoorState == EDoorState::Closing)
	{
		TArray<AActor*> OverlappingActors;
		SafetyBox->GetOverlappingActors(OverlappingActors, APawn::StaticClass());
		if (!OverlappingActors.IsEmpty())
		{
			if (CancelCloseOnSafetyZoneOverlap)
			{
				Open();
			}
			PushActorsOutOfSafetyBox(OverlappingActors, DeltaSeconds);
		}
		else if (WantsToSetSafetyZoneToBlocking)
		{
			SafetyBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
			SetActorTickEnabled(false);
			WantsToSetSafetyZoneToBlocking = false;
		}
	}
	
}

/** Pushes actors out of the safety box when the door is closing. */
void ASlidingDoor::PushActorsOutOfSafetyBox(TArray<AActor*> Actors, const float DeltaTime)
{
	constexpr float PushStrength {400.0f};
	
	for (AActor* OverlappingActor : Actors)
	{
		if (APawn* OverlappingPawn = Cast<APawn>(OverlappingActor))
		{
			FVector PawnLocation {OverlappingPawn->GetActorLocation()};
			FVector PushDirection {(PawnLocation - GetActorLocation()).GetSafeNormal()};
			const FVector PushForce {PushDirection * PushStrength * DeltaTime};

			OverlappingPawn->AddActorWorldOffset(PushForce, true);
		}
	}
}

void ASlidingDoor::StartCooldown()
{
	IsCooldownActive = true;
	if (const UWorld* World = GetWorld())
	{
		FTimerManager& TimerManager = World->GetTimerManager();
		TimerManager.SetTimer(CooldownTimerHandle, this, &ASlidingDoor::HandleCooldownFinished, CooldownTime, false);
	}
}

void ASlidingDoor::SetSafetyZoneCollisionEnabled(const bool Value)
{
	if (Value)
	{
		TArray<AActor*> OverlappingActors;
		SafetyBox->GetOverlappingActors(OverlappingActors, APawn::StaticClass());
		if (!OverlappingActors.IsEmpty())
		{
			WantsToSetSafetyZoneToBlocking = true;
			SetActorTickEnabled(true);
			return;
		}
		SafetyBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
		return;
	}
	
	SetActorTickEnabled(false);
	
	if (SafetyBox->GetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn) == ECR_Block)
	{
		SafetyBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Overlap);
	}
}

void ASlidingDoor::HandleCooldownFinished()
{
	IsCooldownActive = false;
}

void ASlidingDoor::HandleCloseTimerUpdate()
{
	{
		Close();
	}
}

void ASlidingDoor::Open_Implementation()
{
	if (IsLocked) { return; }
	
	if (DoorState == EDoorState::Closed || DoorState == EDoorState::Closing)
	{
		if (RequiresPower && PowerConsumerComponent && PowerConsumerComponent->GetIsPowered() || !RequiresPower)
		{
			if (GetWorld()->GetTimerManager().IsTimerActive(CloseCheckTimerHandle))
			{
				GetWorld()->GetTimerManager().ClearTimer(CloseCheckTimerHandle);
			}
		
			DoorState = EDoorState::Opening;

			SetSafetyZoneCollisionEnabled(false);
	
			OnDoorStateChanged.Broadcast(EDoorState::Opening);
			
			return;
		}
	}
}

void ASlidingDoor::Close_Implementation()
{
	if (IsLocked && !CloseOnLock) { return; }
	
	if (DoorState == EDoorState::Open || DoorState == EDoorState::Opening)
	{
		if (RequiresPower && PowerConsumerComponent && PowerConsumerComponent->GetIsPowered() || !RequiresPower)
		{
			if (CanClose())
			{
				DoorState = EDoorState::Closing;
				OnDoorStateChanged.Broadcast(EDoorState::Closing);

				SetActorTickEnabled(true);
				
				return;
			}
			if (const UWorld* World {GetWorld()})
			{
				FTimerManager& TimerManager = World->GetTimerManager();
				TimerManager.SetTimer(CloseCheckTimerHandle, this, &ASlidingDoor::HandleCloseTimerUpdate, 0.5f, false);
			}
		}
	}
}

void ASlidingDoor::Lock_Implementation()
{
	if (DoorState == EDoorState::Closed || DoorState ==  EDoorState::Closing)
	{
		StateWhenLocked = EDoorState::Closed;		
	}
	else if (DoorState == EDoorState::Open || DoorState ==  EDoorState::Opening)
	{
		StateWhenLocked = EDoorState::Open;
	}
	
	if (CloseOnLock)
	{
		Close();
	}
	IsLocked = true;
}

void ASlidingDoor::Unlock_Implementation()
{
	IsLocked = false;

	if (!DoActionOnUnlock) { return; }
	if (ActionOnUnlock == EDoorAction::Open)
	{
		Open();
	}
	else if (ActionOnUnlock == EDoorAction::Close)
	{
		Close();
	}
	else if (ActionOnUnlock == EDoorAction::Reset)
	{
		if (StateWhenLocked == EDoorState::Open)
		{
			Open();
		}
		else
		{
			Close();
		}
	}
}

bool ASlidingDoor::CanClose() const
{
	TArray<AActor*> OverlappingActors;
	SafetyBox->GetOverlappingActors(OverlappingActors, APawn::StaticClass());
	
	if (OverlappingActors.Num() > 0)
	{
		return false;
	}
	return true;
}

void ASlidingDoor::EventDoorOpened_Implementation()
{
	DoorState = EDoorState::Open;
	
	SetSafetyZoneCollisionEnabled(false);
	
	OnDoorStateChanged.Broadcast(EDoorState::Open);
}

void ASlidingDoor::EventDoorClosed_Implementation()
{
	DoorState = EDoorState::Closed;
	
	SetSafetyZoneCollisionEnabled(false);
	
	OnDoorStateChanged.Broadcast(EDoorState::Closed);
}

void ASlidingDoor::EventOnPowerStateChanged_Implementation(const bool NewState)
{
}