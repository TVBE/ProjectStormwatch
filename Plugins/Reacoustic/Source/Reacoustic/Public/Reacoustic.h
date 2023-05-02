// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Logging/LogMacros.h"
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Engine/StaticMesh.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/AssetManager.h"


DECLARE_LOG_CATEGORY_EXTERN(LogReacousticSubsystem, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogReacousticComponent, Log, All);

class FReacousticModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;


	
};
