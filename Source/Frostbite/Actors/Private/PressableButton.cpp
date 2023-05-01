// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

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

#if WITH_EDITOR
	ValidateLinkedButtons();
#endif
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

void APressableButton::DoTargetActorActions(const bool IsPressedAction)
{
	if (IsPressedAction)
	{
		if (ActionsOnPressed.IsEmpty()) { return; }
		for (FActorFunctionCaller& FunctionCaller : ActionsOnPressed)
		{
			FunctionCaller.CallFunction();
		}
	}
	else
	{
		if (ActionsOnRelease.IsEmpty()) { return; }
		for (FActorFunctionCaller& FunctionCaller : ActionsOnRelease)
		{
			FunctionCaller.CallFunction();
		}
	}
}

void APressableButton::DoLinkedButtonActions(const bool IsPressedAction)
{
	if (LinkedButtons.IsEmpty()) { return; }
	if (!IsPressedAction && TriggerType == EButtonTriggerType::SinglePress) {return; }
	
	for (const auto& [Actor,  DoActionOnPressed, PressedAction, DoActionOnRelease, ReleasedAction, IsActionLinked] : LinkedButtons)
	{
		if (Actor.IsValid())
		{
			if (IsPressedAction && DoActionOnPressed || !IsPressedAction && DoActionOnRelease)
			{
				switch (const ELinkedButtonAction Action {IsPressedAction ? PressedAction : ReleasedAction})
				{
				case ELinkedButtonAction::Press:
					{
						if (!Actor->GetIsPressed())
						{
							Actor->EventOnPress(!IsActionLinked, false);
						}
					}
					break;
				case ELinkedButtonAction::Release:
					{
						if (Actor->GetIsPressed())
						{
							Actor->EventOnRelease(!IsActionLinked, false);
						}
					}
					break;
				case ELinkedButtonAction::Toggle:
					{
						if (Actor->GetIsPressed())
						{
							Actor->EventOnRelease(!IsActionLinked, false);
						}
						else
						{
							Actor->EventOnPress(!IsActionLinked, false);
						}
					}
					break;
				default: break;
				}
			}
		}
	}
}

#if WITH_EDITOR
void APressableButton::ValidateLinkedButtons()
{
	if (LinkedButtons.IsEmpty()) { return; }
	for (FLinkedButton& LinkedButton : LinkedButtons)
	{
		if (!LinkedButton.Actor.IsNull())
		{
			if (const AActor* Actor = LinkedButton.Actor.Get(); Actor && Actor == this)
				{
				const FString DisplayName = this->GetHumanReadableName();
				UE_LOG(LogButton, Warning, TEXT("%s contains button link to self."), *DisplayName)

				LinkedButton.Actor = nullptr;

				if (GEditor)
				{
					const FText Title {FText::FromString("Button")};
					const FText Message {FText::FromString("Cannot add button link to self!")};
					FMessageDialog::Open(EAppMsgType::Ok, Message, &Title);
				}
				}
		}
		if (TriggerType == EButtonTriggerType::SinglePress && LinkedButton.DoActionOnRelease)
		{
			if (GEditor)
			{
				const FText Title {FText::FromString("Button")};
				const FText Message {FText::FromString("This button is a single press button and does not implement a release event.")};
				FMessageDialog::Open(EAppMsgType::Ok, Message, &Title);
			}
			LinkedButton.DoActionOnRelease = false;
		}
	}
}
#endif

void APressableButton::EventOnPress_Implementation(const bool CallTargetActors, const bool CallLinkedButtons)
{
	if (CallTargetActors)
	{
		DoTargetActorActions(true);
	}
	
	if (CallLinkedButtons)
	{
		DoLinkedButtonActions(true);
	}
	
}

void APressableButton::EventOnRelease_Implementation(const bool CallTargetActors, const bool CallLinkedButtons)
{
	if (CallTargetActors)
	{
		DoTargetActorActions(false);
	}
	
	if (CallLinkedButtons)
	{
		DoLinkedButtonActions(false);
	}
}

void APressableButton::EventOnCollisionTrigger_Implementation()
{
	if (!IsPressed && !IsCooldownActive)
	{
		EventOnPress(true, true);
	}
}

void APressableButton::EventOnPowerStateChanged_Implementation(bool NewState)
{
}



