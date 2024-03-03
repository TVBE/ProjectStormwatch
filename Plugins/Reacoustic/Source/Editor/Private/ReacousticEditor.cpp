// Copyright (c) 2022-present Nino Saglia. All Rights Reserved.
#include "ReacousticEditor.h"
#include "Editor.h"
#include "AssetToolsModule.h"
#include "ComponentAssetBroker.h"
#include "EditorModeRegistry.h"
#include "ReacousticEditorMode.h"
#include "ReacousticSoundAssetBroker.h"
#include "ReacousticSoundAssetTypeActions.h"
#include "ReacousticStaticMeshDetailCustomization.h"
#include "ActorFactories/ActorFactory.h"
#include "HAL/PlatformFilemanager.h"
#include "Internationalization/Internationalization.h"
#include "Misc/Paths.h"


#define LOCTEXT_NAMESPACE "ReacousticEditor"
IMPLEMENT_MODULE(FReacousticEditorModule, ReacousticEditor)

void FReacousticEditorModule::StartupModule()
{
	/** Startup various asset type actions for reacoustic sounds.*/
	IAssetTools& AssetTools {FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get()};
	ReacousticSoundAssetTypeActions = MakeShared<FReacousticSoundAssetTypeActions>();
	FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(ReacousticSoundAssetTypeActions.ToSharedRef());
	/** -Generate reacoustic sound from wavefile. */
	FReacousticContentBrowserMenuExtension::RegisterMenus();
	
	/** Register custom sidepanel view for the editor mode.*/
	FEditorModeRegistry::Get().RegisterMode<ReacousticEditorMode>(
		ReacousticEditorMode::EM_ReacousticEditorModeId,
		LOCTEXT("ReacousticEditorMode","Reacoustic"),
		FSlateIcon(),
		true 
	);
	
	/** register custom broker that converts reacoustic sound assets to reacoustic components when drag dropint into detail panel.*/
	FComponentAssetBrokerage::RegisterBroker(MakeShareable(new FReacousticSoundAssetBroker()), UReacousticComponent::StaticClass(), true, true);
	
}









void FReacousticEditorModule::ShutdownModule()
{
	if (!FModuleManager::Get().IsModuleLoaded("AssetTools")) return;
	FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(ReacousticSoundAssetTypeActions.ToSharedRef());

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	
	PropertyModule.UnregisterCustomClassLayout("StaticMesh");

	FEditorModeRegistry::Get().UnregisterMode(ReacousticEditorMode::EM_ReacousticEditorModeId);
	
	FComponentAssetBrokerage::UnregisterBroker(MakeShareable(new FReacousticSoundAssetBroker()));
}



#undef LOCTEXT_NAMESPACE

