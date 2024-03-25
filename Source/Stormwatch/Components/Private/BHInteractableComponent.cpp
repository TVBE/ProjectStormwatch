// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHInteractableComponent.h"

float FBHInteractionEvent::Duration() const
{
	if (!ensureAlways(StartTime.IsSet()))
	{
		return 0.0f;
	}
	return ((EndTime.IsSet() ? EndTime.GetValue() : FDateTime::Now()) - StartTime.GetValue()).GetTotalSeconds();
}

bool FBHInteractionEvent::HasStarted() const
{
	return StartTime.IsSet();
}

bool FBHInteractionEvent::HasFinished() const
{
	return StartTime.IsSet() && EndTime.IsSet();
}

bool UBHInteractableComponent::CanBeInteractedWith_Implementation() const
{
	return true;
}

FVector UBHInteractableComponent::GetInteractionLocation_Implementation() const
{
	return GetComponentLocation();
}

int32 UBHInteractableComponent::GetInteractionPriority_Implementation() const
{
	return 0;
}

TOptional<FBHInteractionEvent> UBHInteractableComponent::GetCurrentInteraction() const
{
	return CurrentInteraction;
}

TOptional<FBHInteractionEvent> UBHInteractableComponent::GetLastInteraction() const
{
	return LastInteraction;
}

bool UBHInteractableComponent::IsBeingInteractedWith() const
{
	return CurrentInteraction.IsSet();
}

bool UBHInteractableComponent::HasEverBeenInteractedWith() const
{
	return LastInteraction.IsSet();
}

float UBHInteractableComponent::GetDurationOfCurrentInteraction() const
{
	if (!CurrentInteraction.IsSet())
	{
		return 0.0f;
	}
	return CurrentInteraction.GetValue().Duration();
}

float UBHInteractableComponent::GetTimeSinceLastInteraction() const
{
	if (!LastInteraction.IsSet() || !LastInteraction.GetValue().HasFinished())
	{
		return 0.0f;
	}
	return (FDateTime::Now() - LastInteraction.GetValue().EndTime.GetValue()).GetTotalSeconds();
}

float UBHInteractableComponent::GetDurationOfLastInteraction() const
{
	if (!LastInteraction.IsSet() || !LastInteraction.GetValue().HasFinished())
	{
		return 0.0f;
	}
	return LastInteraction.GetValue().Duration();
}

int32 UBHInteractableComponent::GetInteractionCount() const
{
	return InteractionCount;
}

void UBHInteractableComponent::OnRegister()
{
	Super::OnRegister();
#if WITH_EDITOR
	if (bAllowOnlyOneInstancePerActor && !GIsPlayInEditorWorld)
	{
		if (const AActor* Owner = GetOwner())
		{
			const TSet<UActorComponent*>& Components = Owner->GetComponents();

			const UActorComponent* DuplicateComponent = nullptr;
			for (const auto* Component : Components)
			{
				if (Component != this && Component->GetClass() == this->GetClass())
				{
					DuplicateComponent = Component;
					break;
				}
			}
			
			if (DuplicateComponent)
			{
				FText DialogText = FText::Format(
					NSLOCTEXT("BHInteractableComponent",
						"DuplicateComponentWarning",
						"Cannot add multiple instances of {0} to a single actor. "
						"This may cause unexpected behavior."),
					FText::FromName(FName(GetClass()->GetName()))
				);

				const FText DialogTitle = NSLOCTEXT("BHInteractableComponent", "BHInteractableComponent", "Warning");
				FMessageDialog::Open(EAppMsgType::Ok, DialogText, DialogTitle);
			}
		}
	}
#endif
}

void UBHInteractableComponent::BeginInteraction()
{
	ensureAlways(!CurrentInteraction.IsSet());
	
	InteractionCount++;
	CurrentInteraction.Emplace(FBHInteractionEvent());
}

void UBHInteractableComponent::EndInteraction()
{
	if(!ensureAlways(CurrentInteraction.IsSet()))
	{
		return;
	}

	CurrentInteraction.GetValue().EndTime = FDateTime::Now();
	LastInteraction.Emplace(CurrentInteraction.GetValue());
	CurrentInteraction.Reset();
}
