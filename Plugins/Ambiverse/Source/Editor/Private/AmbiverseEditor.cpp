// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#include "AmbiverseEditor.h"
#include "AssetTypeActions_AmbiverseLayer.h"
#include "AssetTypeActions_AmbiverseParameter.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"

void FAmbiverseEditorModule::StartupModule()
{
	IAssetTools& AssetTools {FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get()};
	
	AmbiverseElementAssetTypeActions = MakeShared<FAssetTypeActions_AmbiverseElement>();
	FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(AmbiverseElementAssetTypeActions.ToSharedRef());
	
	AmbiverseLayerAssetTypeActions = MakeShared<FAssetTypeActions_AmbiverseLayer>();
	FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(AmbiverseLayerAssetTypeActions.ToSharedRef());

	AmbiverseCompositeAssetTypeActions = MakeShared<FAssetTypeActions_AmbiverseComposite>();
	FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(AmbiverseCompositeAssetTypeActions.ToSharedRef());

	AmbiverseParameterAssetTypeActions = MakeShared<FAssetTypeActions_AmbiverseParameter>();
	FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(AmbiverseParameterAssetTypeActions.ToSharedRef());
}

void FAmbiverseEditorModule::ShutdownModule()
{
	if (!FModuleManager::Get().IsModuleLoaded("AssetTools")) return;

	FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(AmbiverseElementAssetTypeActions.ToSharedRef());
	FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(AmbiverseLayerAssetTypeActions.ToSharedRef());
	FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(AmbiverseCompositeAssetTypeActions.ToSharedRef());
	FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(AmbiverseParameterAssetTypeActions.ToSharedRef());
}

IMPLEMENT_MODULE(FAmbiverseEditorModule, AmbiverseEditor)