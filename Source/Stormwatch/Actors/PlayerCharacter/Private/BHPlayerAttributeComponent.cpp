// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHPlayerAttributeComponent.h"

UBHPlayerAttributeComponent::UBHPlayerAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UBHPlayerAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Value < MaxValue && bEnableRegeneration)
	{
		SetComponentTickEnabled(true);
	}
}

void UBHPlayerAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TimeSinceLastDecrement += DeltaTime;

	if (TimeSinceLastDecrement >= RegenerationDelay)
	{
		const float Increment = (RegenerationMethod == EBHAttributeRegenerationMethod::Curve ?
			RegenerationRateCurve.GetRichCurve()->Eval(TimeSinceLastDecrement) : RegenerationRate) * DeltaTime;
		Incrementvalue(Increment);
	}
	if (Value == MaxValue)
	{
		SetComponentTickEnabled(false);
	}
}

void UBHPlayerAttributeComponent::DecrementValue(float Amount, bool bClamp, float Floor)
{
	Amount = FMath::Max(Amount, 0.0f);
	if (bClamp && Value - Amount < Floor)
	{
		SetValue(Floor);
	}
	else
	{
		SetValue(FMath::Max(Value - Amount, 0.0f));
	}
}

void UBHPlayerAttributeComponent::Incrementvalue(float Amount, bool bClamp, float Ceiling)
{
	Amount = FMath::Max(Amount, 0.0f);
	if (bClamp && Value + Amount > Ceiling)
	{
		SetValue(Ceiling);
	}
	else
	{
		SetValue(FMath::Min(Amount, MaxValue));
	}
}

float UBHPlayerAttributeComponent::GetValue() const
{
	return Value;
}

void UBHPlayerAttributeComponent::SetValue(float NewValue)
{
	const float OldValue = Value;
	Value = FMath::Clamp(NewValue, 0.0f, MaxValue);
	OnValueChanged.Broadcast(Value);
	if (OldValue != MaxValue && Value == MaxValue)
	{
		OnMaxValueReached.Broadcast();
	}
	else if (Value == 0.0f)
	{
		OnMinValueReached.Broadcast();
	}
}

float UBHPlayerAttributeComponent::GetMaxValue() const
{
	return MaxValue;
}

void UBHPlayerAttributeComponent::SetMaxValue(float NewValue)
{
	MaxValue = NewValue;
	SetValue(FMath::Min(NewValue, MaxValue));
}

bool UBHPlayerAttributeComponent::IsRegenerationEnabled() const
{
	return bEnableRegeneration;
}

void UBHPlayerAttributeComponent::SetRegenerationEnabled(bool bEnabled)
{
	if (bEnabled && Value < MaxValue)
	{
		SetComponentTickEnabled(true);
	}
	else if (!bEnabled && IsComponentTickEnabled())
	{
		SetComponentTickEnabled(false);
	}
	bEnableRegeneration = bEnabled;
}

void UBHPlayerAttributeComponent::ResetRegeneration()
{
	TimeSinceLastDecrement = 0.0f;
}

