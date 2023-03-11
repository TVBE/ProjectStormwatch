// Copyright 2023 Barrelhouse

#include "PlayerCharacterState.h"

void APlayerCharacterState::ResetPlayerState()
{
	Health = 100;
	Exertion = 0;
	Fear = 0;
	Vigilance = 0;
}

uint8 APlayerCharacterState::IncrementValue(uint8& Property, uint8 Value)
{
	Value = FMath::Clamp(Value, 0, 100);
	if(Property + Value > 100)
	{
		Property = 100;
	}
	else
	{
		Property += Value;
	}
	return Property;
}

uint8 APlayerCharacterState::DecrementValue(uint8& Property, uint8 Value)
{
	Value = FMath::Clamp(Value, 0, 100);
	if(Property < Value)
	{
		Property = 0;
	}
	else
	{
		Property -= Value;
	}
	return Property;
}

uint8 APlayerCharacterState::SetValue(uint8& Property, uint8 Value)
{
	Value = FMath::Clamp(Value, 0, 100);
	Property = Value;
	return Property;
}

	
