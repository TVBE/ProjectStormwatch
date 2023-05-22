// Copyright (c) 2022-present Tim Verberne
// This source code is part of the Adaptive Ambience System plugin

#include "AmbiverseParameterManager.h"
#include "AmbiverseLayerManager.h"
#include "AmbiverseParameter.h"
#include "AmbiverseSubsystem.h"

DEFINE_LOG_CATEGORY_CLASS(UAmbiverseParameterManager, LogAmbiverseParameterManager);

void UAmbiverseParameterManager::Initialize(UAmbiverseSubsystem* Subsystem)
{
	AmbiverseSubsystem = Subsystem;
}

void UAmbiverseParameterManager::Deinitialize()
{
}

void UAmbiverseParameterManager::GetScalarsForEntry(float& DensityScalar, float& VolumeScalar, const UAmbiverseLayer* Layer, const FAmbiverseLayerQueueEntry& Entry)
{
	DensityScalar = 1.0f;
	VolumeScalar = 1.0f;
	
	if (!Layer) { return; }
	
	TArray<FAmbiverseParameterModifiers> Parameters;
	
	Parameters.Append(Layer->Parameters);
	Parameters.Append(Entry.SoundData.Parameters);
	
	if (Parameters.IsEmpty()) { return; }

	for (const FAmbiverseParameterModifiers& Modifier : Parameters)
	{
		UAmbiverseParameter* RegisteredParameter {nullptr};
		
		for (UAmbiverseParameter* Parameter : ParameterRegistry)
		{
			if (Parameter == Modifier.Parameter)
			{
				RegisteredParameter = Parameter;
				break;
			}
		}

		if (!RegisteredParameter)
		{
			RegisterParameter(Modifier.Parameter);
			continue;
		}

		DensityScalar *= FMath::GetMappedRangeValueClamped(FVector2D(0, 1), Modifier.DensityRange, RegisteredParameter->ParameterValue);
		VolumeScalar *= FMath::GetMappedRangeValueClamped(FVector2D(0, 1), Modifier.VolumeRange, RegisteredParameter->ParameterValue);
	}

	DensityScalar *= Layer->LayerDensity;
	DensityScalar = 1 / DensityScalar;
	VolumeScalar *= Layer->LayerVolume;
}

void UAmbiverseParameterManager::SetParameterValue(UAmbiverseParameter* Parameter, const float Value)
{
	if (ParameterRegistry.IsEmpty())
	{
		RegisterParameter(Parameter);
	}

	for (UAmbiverseParameter* AmbiverseParameter : ParameterRegistry)
	{
		if (AmbiverseParameter == Parameter)
		{
			AmbiverseParameter->SetParameter(Value);
			break;
		}
	}

	OnParameterChangedDelegate.Broadcast(Parameter);
}

void UAmbiverseParameterManager::RegisterParameter(UAmbiverseParameter* Parameter)
{
	if (!Parameter) { return; }
	if (!IsParameterRegistered(Parameter))
	{
		ParameterRegistry.Add(Parameter);
		UE_LOG(LogAmbiverseParameterManager, Verbose, TEXT("Registered parameter: '%s'"), *Parameter->GetName())
	}
}

bool UAmbiverseParameterManager::IsParameterRegistered(const UAmbiverseParameter* Parameter) const
{
	if (!Parameter) { return false; }
	if (ParameterRegistry.Contains(Parameter))
	{
		return true;
	}
	return false;
}

