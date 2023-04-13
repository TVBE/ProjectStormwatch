// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#include "PressableButton.h"

DEFINE_LOG_CATEGORY_CLASS(APressableButton, LogButton)

APressableButton::APressableButton()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

inline bool IsLinkedButtonValid(const FLinkedButton& LinkedButton, const APressableButton* Button)
{
	if (LinkedButton.Actor.IsNull())
	{
		return false;
	}
	if (LinkedButton.Actor.Get() == Button)
	{
		return false;
	}
	return true;
}

void APressableButton::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void APressableButton::BeginPlay()
{
	Super::BeginPlay();
	
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
	for (const auto& [Actor, PressedAction, ReleasedAction] : TargetActors)
	{
		if (Actor.IsValid())
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

void APressableButton::DoLinkedButtonActions(const bool IsPressedAction)
{
	if (!IsPressedAction && TriggerType == EButtonTriggerType::SinglePress) {return; }
	
	for (const auto& [Actor, DoActionOnPressed, PressedAction, DoActionOnRelease, ReleasedAction, IsActionLinked] : LinkedButtons)
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

void APressableButton::EventOnPress_Implementation(const bool CallTargetActors, const bool CallLinkedButtons)
{
	if (LinkedButtons.IsEmpty()) { return; }
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
	if (LinkedButtons.IsEmpty()) { return; }
	if (CallTargetActors)
	{
		DoTargetActorActions(false);
	}
	
	if (CallLinkedButtons)
	{
		DoLinkedButtonActions(false);
	}
}



