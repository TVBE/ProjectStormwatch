// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHPlayerStateComponent.h"

void UBHPlayerStateComponent::OnComponentCreated()
{
	Super::OnComponentCreated();
}

void UBHPlayerStateComponent::BeginPlay()
{
	Super::BeginPlay();

	/** Start updating the player state. */
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(StateTimer, this, &UBHPlayerStateComponent::UpdatePlayerState, 1.0f, true);	
	}
}

void UBHPlayerStateComponent::ResetPlayerState()
{
	Pain = 100.0f;
	Exertion = 0.0f;
	Fear = 0.0f;
	Vigilance = 0.0f;
}

float UBHPlayerStateComponent::IncrementValue(float& Property, float Value)
{
	Value = FMath::Clamp(Value, 0, 100);
	if (Property + Value > 100)
	{
		Property = 100;
	}
	else
	{
		Property += Value;
	}
	return Property;
}

float UBHPlayerStateComponent::DecrementValue(float& Property, float Value)
{
	Value = FMath::Clamp(Value, 0, 100);
	if (Property < Value)
	{
		Property = 0;
	}
	else
	{
		Property -= Value;
	}
	return Property;
}

float UBHPlayerStateComponent::SetValue(float& Property, float Value)
{
	Value = FMath::Clamp(Value, 0, 100);
	Property = Value;
	return Property;
}

float UBHPlayerStateComponent::IncrementStateValue(const EBHPlayerStateValue Type, const float Value)
{
	switch(Type)
	{
	case EBHPlayerStateValue::Pain:
		if (Value <= 0.0f) {return Pain; }
		IncrementValue(Pain, Value);
		OnPainChanged.Broadcast(Pain);
		return Pain;
		
	case EBHPlayerStateValue::Exertion:
		if (Value <= 0.0f) {return Exertion; }
		IncrementValue(Exertion, Value);
		OnExertionChanged.Broadcast(Exertion);
		return Exertion;
		
	case EBHPlayerStateValue::Fear:
		if (Value <= 0.0f) {return Fear; }
		IncrementValue(Fear, Value);
		OnFearChanged.Broadcast(Fear);
		return Fear;
		
	case EBHPlayerStateValue::Vigilence:
		if (Value <= 0.0f) {return Vigilance; }
		IncrementValue(Vigilance, Value);
		OnVigilanceChanged.Broadcast(Vigilance);
		return Vigilance;
	}
	return 0.0f;
}

float UBHPlayerStateComponent::DecrementStateValue(const EBHPlayerStateValue Type, const float Value)
{
	switch(Type)
	{
	case EBHPlayerStateValue::Pain:
		if (Value <= 0.0f) {return Pain; }
		DecrementValue(Pain, Value);
		OnPainChanged.Broadcast(Pain);
		return Pain;
		
	case EBHPlayerStateValue::Exertion:
		if (Value <= 0.0f) {return Exertion; }
		DecrementValue(Exertion, Value);
		OnExertionChanged.Broadcast(Exertion);
		return Exertion;
		
	case EBHPlayerStateValue::Fear:
		if (Value <= 0.0f) {return Fear; }
		DecrementValue(Fear, Value);
		OnFearChanged.Broadcast(Fear);
		return Fear;
		
	case EBHPlayerStateValue::Vigilence:
		if (Value <= 0.0f) {return Vigilance; }
		DecrementValue(Vigilance, Value);
		OnVigilanceChanged.Broadcast(Vigilance);
		return Vigilance;
	}
	return 0.0f;
}

float UBHPlayerStateComponent::SetStateValue(const EBHPlayerStateValue Type, const float Value)
{
	switch(Type)
	{
	case EBHPlayerStateValue::Pain:
		SetValue(Pain, Value);
		OnPainChanged.Broadcast(Pain);
		return Pain;
		
	case EBHPlayerStateValue::Exertion:
		SetValue(Exertion, Value);
		OnExertionChanged.Broadcast(Exertion);
		return Exertion;
		
	case EBHPlayerStateValue::Fear:
		SetValue(Fear, Value);
		OnFearChanged.Broadcast(Fear);
		return Fear;
		
	case EBHPlayerStateValue::Vigilence:
		if (Value <= 0.0f) {return Vigilance; }
		SetValue(Vigilance, Value);
		OnVigilanceChanged.Broadcast(Vigilance);
		return Vigilance;
	}
	return 0.0f;
}

void UBHPlayerStateComponent::UpdatePlayerState()
{
	EventOnUpdate();
}

void UBHPlayerStateComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	/** Stop updating the player state. */
	StateTimer.Invalidate();
}

void UBHPlayerStateComponent::EventOnUpdate_Implementation()
{
}


	
