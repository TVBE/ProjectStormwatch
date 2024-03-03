#include "ReacousticAssociationEditorWidget.h"
#include "Engine/Selection.h"
#include "EngineUtils.h"
#include "ReacousticComponent.h"
#include "ReacousticSoundAsset.h"
#include "Widgets/SWindow.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Engine/StaticMeshActor.h"
#include "Logging/LogMacros.h"


DECLARE_LOG_CATEGORY_EXTERN(ReacousticEditorLog, Log, All);
DEFINE_LOG_CATEGORY(ReacousticEditorLog);

#define LOCTEXT_NAMESPACE "ReacousticAssociationEditorWidget"

void ReacousticAssociationEditorWidget::Construct(const FArguments& InArgs)
{
	UE_LOG(ReacousticEditorLog, Log, TEXT("Construct called"));

	if (!ScrollBoxWidget.IsValid())
	{
		ChildSlot
		[
			SAssignNew(ScrollBoxWidget, SScrollBox)
			+ SScrollBox::Slot()
			.VAlign(VAlign_Top)
			.HAlign(HAlign_Left)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("Header", "Actors in Level:"))
			]
		];
		UE_LOG(ReacousticEditorLog, Log, TEXT("ScrollBoxWidget has been created"));

	}

	UReacousticSubsystem* ReacousticSubsystem = GEditor->GetEditorWorldContext().World()->GetSubsystem<UReacousticSubsystem>();
	if (!ReacousticSubsystem)
	{
		UE_LOG(ReacousticEditorLog, Error, TEXT("ReacousticSubsystem not found"));
		return;
	}

	int32 ActorCount = 0;
	

	/** Loop through all AStaticMeshActor actors in the level */
	for (TActorIterator<AStaticMeshActor> ActorIt(GEditor->GetEditorWorldContext().World()); ActorIt; ++ActorIt)
	{
		

		AStaticMeshActor* CurrentActor = *ActorIt;

		UReacousticComponent* SoundComponent = CurrentActor->FindComponentByClass<UReacousticComponent>();
		
		AddActorToList(CurrentActor,
		bool(SoundComponent),
		false,ReacousticSubsystem->GetMeshSoundAsset(CurrentActor->GetStaticMeshComponent()));
	}


	UE_LOG(ReacousticEditorLog, Log, TEXT("Processed %d AStaticMeshActor actors"), ActorCount);
}

void ReacousticAssociationEditorWidget::AddToList(const FString& Info)
{
}

void ReacousticAssociationEditorWidget::AddActorToList(AActor* Actor, bool hascomponent,
	bool IsGenerateHitEventsSnabled, UReacousticSoundAsset* SoundAssociation)
{
}

#undef LOCTEXT_NAMESPACE
