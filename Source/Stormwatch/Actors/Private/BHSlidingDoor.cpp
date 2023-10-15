// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHSlidingDoor.h"
#include "BHPowerConsumptionComponent.h"
#include "Components/BoxComponent.h"

ABHSlidingDoor::ABHSlidingDoor()
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

void ABHSlidingDoor::BeginPlay()
{
	Super::BeginPlay();

	DoorState = StartingState;

	if (RequiresPower)
	{
		PowerConsumerComponent = Cast<UBHPowerConsumptionComponent>
		(AddComponentByClass(UBHPowerConsumptionComponent::StaticClass(), false, FTransform(), true));
		if (PowerConsumerComponent)
		{
			PowerConsumerComponent->PowerSource = PowerSource;
			PowerConsumerComponent->OnPowerStateChanged.AddDynamic(this, &ABHSlidingDoor::EventOnPowerStateChanged);

			PowerConsumerComponent->RegisterComponent();
			PowerConsumerComponent->InitializeComponent();
		}
	}
}

void ABHSlidingDoor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (DoorState == EBHDoorState::Closing)
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
void ABHSlidingDoor::PushActorsOutOfSafetyBox(TArray<AActor*> Actors, const float DeltaTime)
{
	constexpr float PushStrength = 400.0f;
	
	for (AActor* OverlappingActor : Actors)
	{
		if (APawn* OverlappingPawn = Cast<APawn>(OverlappingActor))
		{
			FVector PawnLocation = OverlappingPawn->GetActorLocation();
			FVector PushDirection = (PawnLocation - GetActorLocation()).GetSafeNormal();
			const FVector PushForce = PushDirection * PushStrength * DeltaTime;

			OverlappingPawn->AddActorWorldOffset(PushForce, true);
		}
	}
}

void ABHSlidingDoor::StartCooldown()
{
	IsCooldownActive = true;
	if (const UWorld* World = GetWorld())
	{
		FTimerManager& TimerManager = World->GetTimerManager();
		TimerManager.SetTimer(CooldownTimerHandle, this, &ABHSlidingDoor::HandleCooldownFinished, CooldownTime, false);
	}
}

void ABHSlidingDoor::SetSafetyZoneCollisionEnabled(const bool Value)
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

void ABHSlidingDoor::HandleCooldownFinished()
{
	IsCooldownActive = false;
}

void ABHSlidingDoor::HandleCloseTimerUpdate()
{
	{
		Close();
	}
}

void ABHSlidingDoor::Open_Implementation()
{
	if (IsLocked) { return; }
	
	if (DoorState == EBHDoorState::Closed || DoorState == EBHDoorState::Closing)
	{
		if (RequiresPower && PowerConsumerComponent && PowerConsumerComponent->GetIsPowered() || !RequiresPower)
		{
			if (GetWorld()->GetTimerManager().IsTimerActive(CloseCheckTimerHandle))
			{
				GetWorld()->GetTimerManager().ClearTimer(CloseCheckTimerHandle);
			}
		
			DoorState = EBHDoorState::Opening;

			SetSafetyZoneCollisionEnabled(false);
	
			OnDoorStateChanged.Broadcast(EBHDoorState::Opening);
			
			return;
		}
	}
}

void ABHSlidingDoor::Close_Implementation()
{
	if (IsLocked && !CloseOnLock) { return; }
	
	if (DoorState == EBHDoorState::Open || DoorState == EBHDoorState::Opening)
	{
		if (RequiresPower && PowerConsumerComponent && PowerConsumerComponent->GetIsPowered() || !RequiresPower)
		{
			if (CanClose())
			{
				DoorState = EBHDoorState::Closing;
				OnDoorStateChanged.Broadcast(EBHDoorState::Closing);

				SetActorTickEnabled(true);
				
				return;
			}
			if (const UWorld* World = GetWorld())
			{
				FTimerManager& TimerManager = World->GetTimerManager();
				TimerManager.SetTimer(CloseCheckTimerHandle, this, &ABHSlidingDoor::HandleCloseTimerUpdate, 0.5f, false);
			}
		}
	}
}

void ABHSlidingDoor::Lock_Implementation()
{
	if (DoorState == EBHDoorState::Closed || DoorState ==  EBHDoorState::Closing)
	{
		StateWhenLocked = EBHDoorState::Closed;		
	}
	else if (DoorState == EBHDoorState::Open || DoorState ==  EBHDoorState::Opening)
	{
		StateWhenLocked = EBHDoorState::Open;
	}
	
	if (CloseOnLock)
	{
		Close();
	}
	IsLocked = true;
}

void ABHSlidingDoor::Unlock_Implementation()
{
	IsLocked = false;

	if (!DoActionOnUnlock) { return; }
	if (ActionOnUnlock == EBHDoorAction::Open)
	{
		Open();
	}
	else if (ActionOnUnlock == EBHDoorAction::Close)
	{
		Close();
	}
	else if (ActionOnUnlock == EBHDoorAction::Reset)
	{
		if (StateWhenLocked == EBHDoorState::Open)
		{
			Open();
		}
		else
		{
			Close();
		}
	}
}

bool ABHSlidingDoor::CanClose() const
{
	TArray<AActor*> OverlappingActors;
	SafetyBox->GetOverlappingActors(OverlappingActors, APawn::StaticClass());
	
	if (OverlappingActors.Num() > 0)
	{
		return false;
	}
	return true;
}

void ABHSlidingDoor::EventOpened_Implementation()
{
	DoorState = EBHDoorState::Open;
	
	SetSafetyZoneCollisionEnabled(false);
	
	OnDoorStateChanged.Broadcast(EBHDoorState::Open);
}

void ABHSlidingDoor::EventClosed_Implementation()
{
	DoorState = EBHDoorState::Closed;
	
	SetSafetyZoneCollisionEnabled(false);
	
	OnDoorStateChanged.Broadcast(EBHDoorState::Closed);
}

void ABHSlidingDoor::EventOnPowerStateChanged_Implementation(const bool NewState)
{
}
