// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#include "PressableButton.h"

APressableButton::APressableButton()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
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

void APressableButton::EventOnPress_Implementation()
{
	if (LinkedButtons.IsEmpty()) { return; }
	for (const FLinkedButton& LinkedButton : LinkedButtons)
	{
		if (LinkedButton.Actor)
		{
			switch (LinkedButton.PressedAction)
			{
			case ELinkedButtonAction::Press:
				{
					if (!LinkedButton.Actor->GetIsPressed())
					{
						LinkedButton.Actor->EventOnPress();
					}
				}
				break;
			case ELinkedButtonAction::Release:
				{
					if (LinkedButton.Actor->GetIsPressed())
					{
						LinkedButton.Actor->EventOnRelease();
					}
				}
				break;
			case ELinkedButtonAction::Toggle:
				{
					if (LinkedButton.Actor->GetIsPressed())
					{
						LinkedButton.Actor->EventOnRelease();
					}
					else
					{
						LinkedButton.Actor->EventOnPress();
					}
				}
				break;
			case ELinkedButtonAction::Custom: break;
			case ELinkedButtonAction::Nothing: break;
			default: break;
			}
		}
	}
}

void APressableButton::EventOnRelease_Implementation()
{
}



