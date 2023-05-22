// Copyright (c) 2022-present Tim Verberne
// This source code is part of the Adaptive Ambience System plugin

#include "AmbiverseParameterManager.h"

DEFINE_LOG_CATEGORY_CLASS(UAmbiverseParameterManager, LogAmbiverseParameterManager);

void UAmbiverseParameterManager::RegisterLayer(UAmbiverseParameter* Parameter)
{
	if (!Parameter) { return; }
	if (!IsParameterRegistered(Parameter))
	{
		ParameterRegistry.Add(Parameter);
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
