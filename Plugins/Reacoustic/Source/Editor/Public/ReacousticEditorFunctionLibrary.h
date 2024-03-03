#pragma once
#include "Kismet2/ComponentEditorUtils.h"
#include "Editor.h"

#if WITH_EDITOR


const TCHAR* LLOCTEXT_NAMESPACE = TEXT("ReacousticEditor");
/**
 * This is a function library for various static in editor functions. 
 */

class ReacousticEditorFunctionLibrary : UBlueprintFunctionLibrary
{
public:

	/**Helper function that enabled the in editor time addition of reacoustic sounds to actors.*/
	UFUNCTION(BlueprintCallable, Category = "Category", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static UReacousticComponent* ReacousticEditorFunctionLibrary::AddReacousticComponentInEditor(AActor* ActorToAddTo, TSubclassOf<UActorComponent> NewComponentClass, UReacousticSoundAsset* SoundAssetIn)
	{
		if (!ActorToAddTo || !NewComponentClass || !SoundAssetIn) { UE_LOG(LogTemp, Error, TEXT("AddComponentInEditor: Failed to Add Component! ActorToAddTo or NewComponentClass undefined or Sound asset undefined!")); return NULL; }

		/** Begin the transaction. */
		TUniquePtr<FScopedTransaction> AddTransaction = MakeUnique<FScopedTransaction>(LOCTEXT("ReacousticUtilities", "Add Component In Editor"));
		ActorToAddTo->Modify();

		/** The name from the asset with prevention of duplicate names.*/
		FName NewComponentName = *FComponentEditorUtils::GenerateValidVariableNameFromAsset(SoundAssetIn, ActorToAddTo);

		/** Get the set of owned components that exists prior to instancing the new component. */
		TInlineComponentArray<UActorComponent*> PreInstanceComponents;
		ActorToAddTo->GetComponents(PreInstanceComponents);
    
			UReacousticComponent* NewComponent;
		NewComponent = NewObject<UReacousticComponent>(ActorToAddTo, NewComponentClass, NewComponentName);
    
		if (NewComponent)
		{
			/** Do Scene Attachment if this new Component is a USceneComponent */
			if (USceneComponent* NewSceneComponent = Cast<USceneComponent>(NewComponent))
			{
				/** Simply attach to Actor's root component. */
				USceneComponent* AttachTo = ActorToAddTo->GetRootComponent();
				check(AttachTo != nullptr);
    
				/** Make sure that the mobility of the new scene component is such that we can attach it */
				if (AttachTo->Mobility == EComponentMobility::Movable)
				{
					NewSceneComponent->Mobility = EComponentMobility::Movable;
				}
				else if (AttachTo->Mobility == EComponentMobility::Stationary && NewSceneComponent->Mobility == EComponentMobility::Static)
				{
					NewSceneComponent->Mobility = EComponentMobility::Stationary;
				}
    
				NewSceneComponent->AttachToComponent(AttachTo, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			}
    
			ActorToAddTo->AddInstanceComponent(NewComponent);
			NewComponent->ReacousticSoundAsset = SoundAssetIn;
			NewComponent->OnComponentCreated();
			NewComponent->RegisterComponent();
			NewComponent->InitializeComponent();
    
			/** Register any new components that may have been created during construction of the instanced component, but were not explicitly registered. */
			TInlineComponentArray<UActorComponent*> PostInstanceComponents;
    	
			ActorToAddTo->GetComponents(PostInstanceComponents);
    	
			for (UActorComponent* ActorComponent : PostInstanceComponents)
			{
				if (!ActorComponent->IsRegistered() && ActorComponent->bAutoRegister && !IsValid(ActorComponent) && !PreInstanceComponents.Contains(ActorComponent))
				{
					ActorComponent->RegisterComponent();
				}
			}
			/** Refresh the actor if it's selected so the new component appears right away. */
			if (ActorToAddTo->IsSelected())
			{
				GEditor->SelectActor(ActorToAddTo, false, true);
				GEditor->SelectActor(ActorToAddTo, true, true);
			}
			return NewComponent;
		}
	return NULL;
	}
};
#endif