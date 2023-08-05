// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#include "AmbiverseParameterManager.h"
#include "AmbiverseLayerManager.h"
#include "AmbiverseParameterAsset.h"
#include "AmbiverseSubsystem.h"

DEFINE_LOG_CATEGORY_CLASS(UAmbiverseParameterManager, LogAmbiverseParameterManager);

void UAmbiverseParameterManager::Initialize(UAmbiverseSubsystem* Subsystem)
{
	Super::Initialize(Subsystem);
	
	if (UAmbiverseLayerManager* LayerManager {Subsystem->GetLayerManager()})
	{
		LayerManager->OnLayerRegistered.AddDynamic(this, &UAmbiverseParameterManager::HandleOnLayerRegistered);
	}
}

float UAmbiverseParameterManager::GetParameterValue(UAmbiverseParameterAsset* Parameter)
{
	if (!Parameter) { return 1.0f; }
	
	for (UAmbiverseParameterAsset* ActiveParameter : ParameterRegistry)
	{
		if (Parameter == ActiveParameter)
		{
			return ActiveParameter->ParameterValue;
		}
	}

	RegisterParameter(Parameter);
	return Parameter->ParameterValue;
}

void UAmbiverseParameterManager::SetParameterValue(UAmbiverseParameterAsset* Parameter, const float Value)
{
	if (ParameterRegistry.IsEmpty())
	{
		RegisterParameter(Parameter);
	}

	for (UAmbiverseParameterAsset* AmbiverseParameter : ParameterRegistry)
	{
		if (AmbiverseParameter == Parameter)
		{
			AmbiverseParameter->SetParameter(Value);
			break;
		}
	}

	OnParameterChangedDelegate.Broadcast(Parameter);
}

void UAmbiverseParameterManager::RegisterParameter(UAmbiverseParameterAsset* Parameter)
{
	if (!Parameter) { return; }
	if (!IsParameterRegistered(Parameter))
	{
		ParameterRegistry.Add(Parameter);
		UE_LOG(LogAmbiverseParameterManager, Verbose, TEXT("Registered parameter: '%s'"), *Parameter->GetName())
	}
}

bool UAmbiverseParameterManager::IsParameterRegistered(const UAmbiverseParameterAsset* Parameter) const
{
	if (!Parameter) { return false; }
	if (ParameterRegistry.Contains(Parameter))
	{
		return true;
	}
	return false;
}

void UAmbiverseParameterManager::HandleOnSceneRegistered(UAmbiverseSceneAsset* RegisteredScene)
{
	if (!RegisteredScene) { return; }

	UE_LOG(LogAmbiverseParameterManager, Verbose, TEXT("Updating parameter registry for: '%s'"), *RegisteredScene->GetName());

	TArray<UAmbiverseParameterAsset*> RequiredParameters;

	for (FAmbiverseParameterModifiers& ParameterModifiers : RegisteredScene->Parameters)
	{
		RequiredParameters.AddUnique(ParameterModifiers.Parameter);
	}

	// for (FAmbiverseProceduralElement ProceduralSoundData : RegisteredScene->ProceduralSounds)
	// {
	// 	for (FAmbiverseParameterModifiers& ParameterModifiers : ProceduralSoundData.Parameters)
	// 	{
	// 		RequiredParameters.AddUnique(ParameterModifiers.Parameter);
	// 	}
	// }

	for (UAmbiverseParameterAsset* RequiredParameter : RequiredParameters)
	{
		if (!ParameterRegistry.Contains(RequiredParameter))
		{
			RegisterParameter(RequiredParameter);
			UE_LOG(LogAmbiverseParameterManager, Verbose, TEXT("Registered parameter: '%s'"), *RequiredParameter->GetName());
		}
		else
		{
			UE_LOG(LogAmbiverseParameterManager, Verbose, TEXT("Parameter already registered: '%s'"), *RequiredParameter->GetName());
		}
	}
}

void UAmbiverseParameterManager::Deinitialize(UAmbiverseSubsystem* Subsystem)
{
	if (!Subsystem) { return; }
	
	if (UAmbiverseLayerManager* LayerManager {Subsystem->GetLayerManager()})
	{
		LayerManager->OnLayerRegistered.RemoveDynamic(this, &UAmbiverseParameterManager::HandleOnLayerRegistered);
	}

	Super::Deinitialize(Subsystem);
}

