// Copyright (c) 2022-present Nino Saglia. All Rights Reserved.

#include "ReacousticEditor.h"
#include "ReacousticSoundAssetTypeActions.h"
#include "AssetToolsModule.h"
#include "ReacousticStaticMeshDetailCustomization.h"
#include "IAssetTools.h"

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
    
		// This registers your customization for UStaticMesh objects, assuming you're trying to customize the details of UStaticMesh.
		PropertyModule.RegisterCustomClassLayout("StaticMesh", FOnGetDetailCustomizationInstance::CreateStatic(&ReacousticStaticMeshDetailCustomization::MakeInstance));
		UE_LOG(LogTemp, Log, TEXT("Registered custom class layout for StaticMesh."));
    
		// If you have other customizations, register them similarly
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load PropertyEditor module."));
	}

}

void FReacousticEditorModule::ShutdownModule()
{
	if (!FModuleManager::Get().IsModuleLoaded("AssetTools")) return;
	FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(ReacousticSoundAssetTypeActions.ToSharedRef());

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    
	// Make sure to unregister during shutdown
	PropertyModule.UnregisterCustomClassLayout("StaticMesh");
}

IMPLEMENT_MODULE(FReacousticEditorModule, ReacousticEditor)