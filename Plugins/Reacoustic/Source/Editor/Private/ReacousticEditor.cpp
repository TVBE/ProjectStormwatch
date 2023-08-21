// Copyright (c) 2022-present Nino Saglia. All Rights Reserved.

#include "ReacousticEditor.h"
#include "ReacousticSoundAssetTypeActions.h"
#include "Internationalization/Internationalization.h"
#include "AssetToolsModule.h"
#include "ReacousticStaticMeshDetailCustomization.h"
#include "IAssetTools.h"
#include "EditorModeRegistry.h"
#include "ReacousticEditorMode.h"

#define LOCTEXT_NAMESPACE "ReacousticEditor"
IMPLEMENT_MODULE(FReacousticEditorModule, ReacousticEditor)

void FReacousticEditorModule::StartupModule()
{
	/** Startup various asset type actions for reacoustic sounds.
	 * -Generate sound from static mesh.
	 * -Preview sound play button.
	 * -Drag and drop action.
	 */
	IAssetTools& AssetTools {FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get()};
		ReacousticSoundAssetTypeActions = MakeShared<FReacousticSoundAssetTypeActions>();
	FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(ReacousticSoundAssetTypeActions.ToSharedRef());
	FReacousticContentBrowserMenuExtension::RegisterMenus();
	
	// Register custom detail views
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	if(&PropertyModule)
	{
		UE_LOG(LogTemp, Log, TEXT("Successfully loaded PropertyEditor module."));
		
		PropertyModule.RegisterCustomClassLayout("StaticMesh", FOnGetDetailCustomizationInstance::CreateStatic(&ReacousticStaticMeshDetailCustomization::MakeInstance));
		UE_LOG(LogTemp, Log, TEXT("Registered custom class layout for StaticMesh."));

	}



	FEditorModeRegistry::Get().RegisterMode<ReacousticEditorMode>(
		ReacousticEditorMode::EM_ReacousticEditorModeId,
		LOCTEXT("ReacousticEditorMode","Reacoustic"),
		FSlateIcon(),
		true // Whether the mode should be visible in the editor
	);
	
}


void FReacousticEditorModule::ShutdownModule()
{
	if (!FModuleManager::Get().IsModuleLoaded("AssetTools")) return;
	FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(ReacousticSoundAssetTypeActions.ToSharedRef());

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    
	// Make sure to unregister during shutdown
	PropertyModule.UnregisterCustomClassLayout("StaticMesh");

	FEditorModeRegistry::Get().UnregisterMode(ReacousticEditorMode::EM_ReacousticEditorModeId);

}

#undef LOCTEXT_NAMESPACE

