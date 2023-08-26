// Copyright (c) 2022-present Nino Saglia. All Rights Reserved.
#include "ReacousticEditor.h"
#include "ReacousticSoundAssetTypeActions.h"
#include "Internationalization/Internationalization.h"
#include "AssetToolsModule.h"
#include "ReacousticStaticMeshDetailCustomization.h"
#include "IAssetTools.h"
#include "ComponentAssetBroker.h"
#include "ContentBrowserDelegates.h"
#include "ContentBrowserMenuContexts.h"
#include "EditorModeRegistry.h"
#include "IAssetTools.h"
#include "ReacousticEditorMode.h"
#include "ReacousticSoundAssetBroker.h"
#include "ReacousticSoundAssetTypeActions.h"
#include "ReacousticStaticMeshDetailCustomization.h"
#include "HAL/PlatformFilemanager.h"
#include "Internationalization/Internationalization.h"
#include "Misc/Paths.h"


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
	
	/** Register custom sidepanel view for the editor mode.*/
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
	
	/** register custom broker that converts reacoustic sound assets to reacoustic components.*/
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

