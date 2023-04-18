// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#include "PressableButton.h"

#include "MeshCollisionTriggerComponent.h"
#include "PowerConsumerComponent.h"
#include "TriggerableObjectInterface.h"

DEFINE_LOG_CATEGORY_CLASS(APressableButton, LogButton)

APressableButton::APressableButton()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void APressableButton::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

#if WITH_EDITOR
	ValidateTargetActors();
	ValidateLinkedButtons();
#endif

	if (CanCollisionTriggerButton && !CollisionTriggerComponent)
	{
		CollisionTriggerComponent = Cast<UMeshCollisionTriggerComponent>
		(AddComponentByClass(UMeshCollisionTriggerComponent::StaticClass(), false, FTransform(), false));
	}
	else if (!CanCollisionTriggerButton && CollisionTriggerComponent)
	{
		CollisionTriggerComponent->DestroyComponent();
	}

	if (RequiresPower && !PowerConsumerComponent && PowerSource.Get())
	{
		PowerConsumerComponent = Cast<UPowerConsumerComponent>
		(AddComponentByClass(UPowerConsumerComponent::StaticClass(), false, FTransform(), true));
		if (PowerConsumerComponent)
		{
			PowerConsumerComponent->PowerSource = PowerSource;
		}
	}
	else if (!RequiresPower && PowerConsumerComponent)
	{
		PowerConsumerComponent->DestroyComponent();
	}
}

void APressableButton::BeginPlay()
{
	Super::BeginPlay();

	if (PowerConsumerComponent)
	{
		PowerConsumerComponent->OnPowerStateChanged.AddDynamic(this, &APressableButton::EventOnPowerStateChanged);
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
	for (const auto& [Actor, DoActionOnPress, PressedAction, DoActionOnRelease, ReleasedAction] : TargetActors)
	{
		if (Actor.IsValid())
		{
			if (IsPressedAction && DoActionOnPress || !IsPressedAction && DoActionOnRelease)
			{
				switch (const ETriggerableObjectAction Action {IsPressedAction ? PressedAction : ReleasedAction})
				{
				case ETriggerableObjectAction::Trigger:
					{
						if (Actor->GetClass()->ImplementsInterface(UTriggerableObject::StaticClass()))
						{
							ITriggerableObject::Execute_Trigger(Actor.Get(), this);
						}
					}
					break;
				case ETriggerableObjectAction::Untrigger:
					{
						if (Actor->GetClass()->ImplementsInterface(UTriggerableObject::StaticClass()))
						{
							ITriggerableObject::Execute_Untrigger(Actor.Get(), this);
						}
					}
					break;
				default: break;
				}
			}
		}
	}
}

void APressableButton::DoLinkedButtonActions(const bool IsPressedAction)
{
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
void APressableButton::ValidateTargetActors()
{
	if (TargetActors.IsEmpty()) { return; }
	for (FButtonTargetActor& TargetActor : TargetActors)
	{
		if (!TargetActor.Actor.IsNull())
		{
			const AActor* Actor = TargetActor.Actor.Get();
			
			if (!Actor->GetClass()->ImplementsInterface(UTriggerableObject::StaticClass()))
			{
				TargetActor.Actor = nullptr;

				if (GEditor)
				{
					const FText Title {FText::FromString("Button")};
					const FText Message {FText::FromString("Target does not implement UTriggerableObject interface. The button cannot perform any actions on this actor. ")};
					FMessageDialog::Open(EAppMsgType::Ok, Message, &Title);
				}
			}
		}
		if (TriggerType == EButtonTriggerType::SinglePress && TargetActor.DoActionOnRelease)
		{
			if (GEditor)
			{
				const FText Title {FText::FromString("Button")};
				const FText Message {FText::FromString("This button is a single press button and does not implement a release event.")};
				FMessageDialog::Open(EAppMsgType::Ok, Message, &Title);
			}
		}
	}
}

void APressableButton::ValidateLinkedButtons()
{
	if (LinkedButtons.IsEmpty()) { return; }
	for (FLinkedButton& LinkedButton : LinkedButtons)
	{
		if (LinkedButton.Actor.Get() == this)
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
		if (TriggerType == EButtonTriggerType::SinglePress && LinkedButton.DoActionOnRelease)
		{
			if (GEditor)
			{
				const FText Title {FText::FromString("Button")};
				const FText Message {FText::FromString("This button is a single press button and does not implement a release event.")};
				FMessageDialog::Open(EAppMsgType::Ok, Message, &Title);
			}
		}
	}
}

#endif

void APressableButton::EventOnPress_Implementation(const bool CallTargetActors, const bool CallLinkedButtons)
{
	if (!TargetActors.IsEmpty() && CallTargetActors)
	{
		DoTargetActorActions(true);
	}
	
	if (!LinkedButtons.IsEmpty() && CallLinkedButtons)
	{
		DoLinkedButtonActions(true);
	}
	
}

void APressableButton::EventOnRelease_Implementation(const bool CallTargetActors, const bool CallLinkedButtons)
{
	if (!TargetActors.IsEmpty() && CallTargetActors)
	{
		DoTargetActorActions(false);
	}
	
	if (!LinkedButtons.IsEmpty() && CallLinkedButtons)
	{
		DoLinkedButtonActions(false);
	}
}

void APressableButton::EventOnPowerStateChanged_Implementation(bool NewState)
{
}



