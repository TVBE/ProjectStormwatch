// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHPressableButton.h"

#include "BHCollisionTriggerComponent.h"
#include "Components/StaticMeshComponent.h"
#include "BHPowerConsumptionComponent.h"
#include "..\..\Interfaces\Public\BHTriggerableObjectInterface.h"

DEFINE_LOG_CATEGORY_CLASS(ABHPressableButton, LogButton)

ABHPressableButton::ABHPressableButton()
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

void ABHPressableButton::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void ABHPressableButton::BeginPlay()
{
	Super::BeginPlay();
	
	/** If the button is configured to be able to be triggered by collisions, add the collision trigger component here. */
	if (CanTriggerByCollision)
	{
		CollisionTriggerComponent = Cast<UBHCollisionTriggerComponent>
		(AddComponentByClass(UBHCollisionTriggerComponent::StaticClass(), true, FTransform(), true));
		
		if (CollisionTriggerComponent && ButtonMesh)
		{
			CollisionTriggerComponent->SetImpulseForceThreshold(CollisionTriggerThreshold);
			CollisionTriggerComponent->bRestrictCollisionAngle = true;
			CollisionTriggerComponent->MaxAllowedAngle = 60.0f;
			
			CollisionTriggerComponent->SetupAttachment(ButtonMesh);
			CollisionTriggerComponent->SetRelativeRotation(FRotator(90, 0, 0));
			
			CollisionTriggerComponent->OnCollisionTrigger.AddDynamic(this, &ABHPressableButton::EventOnCollisionTrigger);

			CollisionTriggerComponent->RegisterComponent();
			CollisionTriggerComponent->InitializeComponent();
		}
	}
		
	/** If the button is configured to use power, add the power consumer component here.  */
	if (RequiresPower)
	{
		PowerConsumerComponent = Cast<UBHPowerConsumptionComponent>
		(AddComponentByClass(UBHPowerConsumptionComponent::StaticClass(), false, FTransform(), true));
		if (PowerConsumerComponent)
		{
			PowerConsumerComponent->PowerSource = PowerSource;
			PowerConsumerComponent->OnPowerStateChanged.AddDynamic(this, &ABHPressableButton::EventOnPowerStateChanged);

			PowerConsumerComponent->RegisterComponent();
			PowerConsumerComponent->InitializeComponent();
		}
	}
}

void ABHPressableButton::StartCooldown()
{
	IsCooldownActive = true;
	if (const UWorld* World = GetWorld())
	{
		FTimerManager& TimerManager = World->GetTimerManager();
		TimerManager.SetTimer(CooldownTimerHandle, this, &ABHPressableButton::HandleCooldownFinished, CooldownTime, false);
	}
}

void ABHPressableButton::HandleCooldownFinished()
{
	IsCooldownActive = false;
}

void ABHPressableButton::EventOnPress_Implementation()
{
	OnButtonPressed.Broadcast();
}

void ABHPressableButton::EventOnRelease_Implementation()
{
	OnButtonReleased.Broadcast();
}

void ABHPressableButton::EventOnCollisionTrigger_Implementation()
{
	if (!IsPressed && !IsCooldownActive)
	{
		EventOnPress();
	}
}

void ABHPressableButton::EventOnPowerStateChanged_Implementation(bool NewState)
{
}



