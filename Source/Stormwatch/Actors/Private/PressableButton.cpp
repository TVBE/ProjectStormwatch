// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "PressableButton.h"

#include "MeshCollisionTriggerComponent.h"
#include "Components/StaticMeshComponent.h"
#include "PowerConsumerComponent.h"
#include "TriggerableObjectInterface.h"

DEFINE_LOG_CATEGORY_CLASS(APressableButton, LogButton)

APressableButton::APressableButton()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = RootSceneComponent;
	
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base"));
	BaseMesh->SetupAttachment(RootComponent);

	ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Button"));
	ButtonMesh->SetupAttachment(BaseMesh);
}

void APressableButton::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void APressableButton::BeginPlay()
{
	Super::BeginPlay();
	
	/** If the button is configured to be able to be triggered by collisions, add the collision trigger component here. */
	if (CanTriggerByCollision)
	{
		CollisionTriggerComponent = Cast<UMeshCollisionTriggerComponent>
		(AddComponentByClass(UMeshCollisionTriggerComponent::StaticClass(), true, FTransform(), true));
		
		if (CollisionTriggerComponent && ButtonMesh)
		{
			CollisionTriggerComponent->SetImpulseForceThreshold(CollisionTriggerThreshold);
			CollisionTriggerComponent->RestrictCollisionAngle = true;
			CollisionTriggerComponent->MaxAllowedAngle = 60.0f;
			
			CollisionTriggerComponent->SetupAttachment(ButtonMesh);
			CollisionTriggerComponent->SetRelativeRotation(FRotator(90, 0, 0));
			
			CollisionTriggerComponent->OnCollisionTrigger.AddDynamic(this, &APressableButton::EventOnCollisionTrigger);

			CollisionTriggerComponent->RegisterComponent();
			CollisionTriggerComponent->InitializeComponent();
		}
	}
		
	/** If the button is configured to use power, add the power consumer component here.  */
	if (RequiresPower)
	{
		PowerConsumerComponent = Cast<UPowerConsumerComponent>
		(AddComponentByClass(UPowerConsumerComponent::StaticClass(), false, FTransform(), true));
		if (PowerConsumerComponent)
		{
			PowerConsumerComponent->PowerSource = PowerSource;
			PowerConsumerComponent->OnPowerStateChanged.AddDynamic(this, &APressableButton::EventOnPowerStateChanged);

			PowerConsumerComponent->RegisterComponent();
			PowerConsumerComponent->InitializeComponent();
		}
	}
}

void APressableButton::StartCooldown()
{
	IsCooldownActive = true;
	if (const UWorld* World = GetWorld())
	{
		FTimerManager& TimerManager = World->GetTimerManager();
		TimerManager.SetTimer(CooldownTimerHandle, this, &APressableButton::HandleCooldownFinished, CooldownTime, false);
	}
}

void APressableButton::HandleCooldownFinished()
{
	IsCooldownActive = false;
}

void APressableButton::EventOnPress_Implementation()
{
	OnButtonPressed.Broadcast();
}

void APressableButton::EventOnRelease_Implementation()
{
	OnButtonReleased.Broadcast();
}

void APressableButton::EventOnCollisionTrigger_Implementation()
{
	if (!IsPressed && !IsCooldownActive)
	{
		EventOnPress();
	}
}

void APressableButton::EventOnPowerStateChanged_Implementation(bool NewState)
{
}



