
#include "ReacousticSoundAssociationSetterFactory.h"

#include "ReacousticComponent.h"
#include "ReacousticSoundAsset.h"
#include "Delegates/DelegateSignatureImpl.inl"
#include "UnrealEd.h"
#include "UObject/UObjectGlobals.h"
#include "ReacousticEditorFunctionLibrary.h"


/**
 * Note: This factory does NOT actually create any actors. It exists solely for the purpose of enabling drag-and-drop functionality
 * for the Reacoustic Sound Asset within the Unreal Editor. When such an asset is dragged and dropped onto an actor,
 * the OnApplyObjectToActor delegate is triggered, allowing custom logic to be executed.
 * This approach is used because Unreal Engine tightly couples drag-and-drop functionality with asset factories.
 */


#if WITH_EDITOR
UReacousticSoundAssociationSetterFactory::UReacousticSoundAssociationSetterFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	/**Subscribe to the delegate that handles drag onto actor interactions.*/
	FEditorDelegates::OnApplyObjectToActor.AddUObject(this, &UReacousticSoundAssociationSetterFactory::TryApplyToActor);
}


/** Returning CanCreateActorFrom as true results in making the asset draggable and therefore triggers the OnApplyObjectToActor delegate.
 *TODO: Find out if there's a more elegant solution for this.*/
bool UReacousticSoundAssociationSetterFactory::CanCreateActorFrom( const FAssetData& AssetData, FText& OutErrorMsg )
{

	if (AssetData.IsValid() && AssetData.IsInstanceOf(UReacousticSoundAsset::StaticClass()))
	{
		ISReaousticSoundvalid = true;
		return true;
	}
	else
		ISReaousticSoundvalid = false;
		OutErrorMsg = NSLOCTEXT("Reacoustic", "CanCreateActorFrom_NoReacousticSound", "No ReacousticSound was specified.");
		return false;

}

/**Gets called by OnApplyObjectToActor to perform the interaction.*/
void UReacousticSoundAssociationSetterFactory::TryApplyToActor(UObject* Object, AActor* Actor)
{
	if (!Object || !Actor) 
	{
		UE_LOG(LogTemp, Error, TEXT("TryApplyToActor: Either Object or Actor is null"));
		return;
	}
	
	if(ISReaousticSoundvalid)
	{
		UE_LOG(LogTemp, Warning, TEXT("ISReaousticSoundvalid is true."));
		
		if (Object->IsA(UReacousticSoundAsset::StaticClass()))
		{

			UReacousticComponent* NewComp{nullptr};
			Actor->Modify();
			if(UReacousticSoundAsset* SoundAsset = {StaticCast<UReacousticSoundAsset*>(Object)})
			{
				/**Helper function that enabled the in editor time addition of reacoustic sounds to actors.*/
				NewComp = ReacousticEditorFunctionLibrary::AddReacousticComponentInEditor(Actor,UReacousticComponent::StaticClass(),SoundAsset);
				NewComp->ReacousticSoundAsset = {StaticCast<UReacousticSoundAsset*>(Object)};
				UE_LOG(LogTemp, Warning, TEXT("UReacousticComponent registered."));
			}
		}
	}
}
#endif


