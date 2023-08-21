#include "ReacousticEditorMode.h"

#include "AITestsCommon.h"
#include "Engine/StaticMeshActor.h"
#include "EngineUtils.h"
#include "ReacousticComponent.h"
#include "ReacousticSoundUserData.h"


const FEditorModeID ReacousticEditorMode::EM_ReacousticEditorModeId = TEXT("EM_ReacousticEditorMode");

void ReacousticEditorMode::Enter()
{
	FEdMode::Enter();


	const FName TabName = FName(TEXT("ReacousticSidePanelTab"));


	if (!FGlobalTabmanager::Get()->HasTabSpawner(TabName))
	{

		FGlobalTabmanager::Get()->RegisterNomadTabSpawner(TabName, FOnSpawnTab::CreateLambda([this](const FSpawnTabArgs& Args)
{
	SidePanelWidget = SNew(ReacousticSidePanel);
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SidePanelWidget.ToSharedRef()
		];
}))

		.SetTooltipText(FText::FromString("Open the Reacoustic Side Panel"));


		FGlobalTabmanager::Get()->TryInvokeTab(TabName);
		

		UpdateHighlightedActorsList();
		}

	FCoreUObjectDelegates::OnObjectPropertyChanged.AddRaw(this, &ReacousticEditorMode::OnPropertyChanged);


		GEditor->RedrawLevelEditingViewports(true);
	}


void ReacousticEditorMode::Exit()
{
	const FName TabName = FName(TEXT("ReacousticSidePanelTab"));


	TSharedPtr<SDockTab> ExistingTab = FGlobalTabmanager::Get()->FindExistingLiveTab(TabName);
	if (ExistingTab.IsValid())
	{
		ExistingTab->RequestCloseTab();
	}


	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(TabName);


	GEditor->SelectNone(true, true);


	GEditor->RedrawLevelEditingViewports(true);
	FEdMode::Exit();
	FCoreUObjectDelegates::OnObjectPropertyChanged.RemoveAll(this);
}

void ReacousticEditorMode::OnPropertyChanged(UObject* Object, FPropertyChangedEvent& PropertyChangedEvent)
{
	int indexToAccess = 0;
	if(PropertyChangedEvent.Property) {
			UpdateHighlightedActorsList();
	}


	
}

void ReacousticEditorMode::UpdateHighlightedActorsList()
{

	HighlightedActors.Empty();
	TArray<TSharedPtr<FReacousticValidityResult>> SharableResultArray;

	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AActor* Actor = *ActorItr;
		if (Actor && !Actor->IsPendingKill())
		{
			UReacousticSubsystem* ReacousticSubsystem = GEditor->GetEditorWorldContext().World()->GetSubsystem<UReacousticSubsystem>();
			FReacousticValidityResult Result = ReacousticSubsystem->CheckReacousticValidity(Actor);
			
			if (Result.HasReacousticComponent || Result.SoundAssociation != nullptr)
			{
				HighlightedActors.Add(Actor);
				
			

				if (SidePanelWidget.IsValid())
				{
					TSharedPtr<FReacousticValidityResult> SharableResult = MakeShareable(new FReacousticValidityResult(Result));
					SharableResultArray.Add(SharableResult);
				}
			}


				SidePanelWidget->UpdateActorList(SharableResultArray);
			}
		}
	}


void ReacousticEditorMode::Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI)
{
	for (AActor* HighlightedActor : HighlightedActors)
	{
		FColor RenderColor{FColor::White};
		UReacousticSubsystem* ReacousticSubsystem = GEditor->GetEditorWorldContext().World()->GetSubsystem<UReacousticSubsystem>();
		FReacousticValidityResult Result = ReacousticSubsystem->CheckReacousticValidity(HighlightedActor);

		if(Result.IsValid())
		{
			if(Result.OverrideAsset)
			{
				RenderColor = FColor::Blue;
			}
			else
			{
				RenderColor = FColor::Green;
			}
		}
		else
		{
			RenderColor = FColor::Orange;
		}
		const FBox BoundingBox = HighlightedActor->GetComponentsBoundingBox(true);
		DrawWireBox(PDI, BoundingBox, RenderColor, SDPG_Foreground);
	}
}


