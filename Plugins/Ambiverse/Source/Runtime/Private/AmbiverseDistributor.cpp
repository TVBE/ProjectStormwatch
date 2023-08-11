// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#include "..\Public\AmbiverseDistributor.h"

DEFINE_LOG_CATEGORY_CLASS(UAmbiverseDistributor, LogAmbiverseDistributor);

void UAmbiverseDistributor::Initialize(const UObject* WorldContextObject)
{
	if (!WorldContextObject) { return; }

	World = WorldContextObject->GetWorld();
}

