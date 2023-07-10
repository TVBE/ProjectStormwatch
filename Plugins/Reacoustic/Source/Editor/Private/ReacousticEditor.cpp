// Copyright (c) 2022-present Nino Saglia. All Rights Reserved.

#include "ReacousticEditor.h"
#include "ReacousticSoundAssetTypeActions.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"

void FReacousticEditorModule::StartupModule()
{
	IAssetTools& AssetTools {FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get()};
	
	UE_LOG(LogTemp, Error, TEXT("Startup"))

	ReacousticSoundAssetTypeActions = MakeShared<FReacousticSoundAssetTypeActions>();
	FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(ReacousticSoundAssetTypeActions.ToSharedRef());
}

void FReacousticEditorModule::ShutdownModule()
{
	if (!FModuleManager::Get().IsModuleLoaded("AssetTools")) return;
	FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(ReacousticSoundAssetTypeActions.ToSharedRef());
}

IMPLEMENT_MODULE(FReacousticEditorModule, ReacousticEditor)