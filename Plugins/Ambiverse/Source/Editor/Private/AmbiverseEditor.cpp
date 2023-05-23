// Copyright (c) 2022-present Tim Verberne
// This source code is part of the Adaptive Ambience System plugin.

#include "AmbiverseEditor.h"
#include "AmbiverseLayerAssetTypeActions.h"
#include "AmbiverseParameterAssetTypeActions.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"

void FAmbiverseEditorModule::StartupModule()
{
	IAssetTools& AssetTools {FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get()};
	
	//  FAssetToolsModule::GetModule().Get().RegisterAdvancedAssetCategory(FName("Sounds|Adaptive Ambience System"),
	//	FText::FromString("Sounds|Adaptive Ambience System"));

	UE_LOG(LogTemp, Error, TEXT("Startup"))

	AmbienceElementAssetTypeActions = MakeShared<FAmbiverseElementAssetTypeActions>();
	FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(AmbienceElementAssetTypeActions.ToSharedRef());
	
	AmbienceLayerAssetTypeActions = MakeShared<FAmbiverseLayerAssetTypeActions>();
	FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(AmbienceLayerAssetTypeActions.ToSharedRef());

	AmbienceCompositeAssetTypeActions = MakeShared<FAmbiverseCompositeAssetTypeActions>();
	FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(AmbienceCompositeAssetTypeActions.ToSharedRef());

	AmbienceParameterAssetTypeActions = MakeShared<FAmbiverseParameterAssetTypeActions>();
	FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(AmbienceParameterAssetTypeActions.ToSharedRef());
}

void FAmbiverseEditorModule::ShutdownModule()
{
	if (!FModuleManager::Get().IsModuleLoaded("AssetTools")) return;

	FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(AmbienceElementAssetTypeActions.ToSharedRef());
	FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(AmbienceLayerAssetTypeActions.ToSharedRef());
	FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(AmbienceCompositeAssetTypeActions.ToSharedRef());
	FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(AmbienceParameterAssetTypeActions.ToSharedRef());
	
}

IMPLEMENT_MODULE(FAmbiverseEditorModule, AmbiverseEditor)