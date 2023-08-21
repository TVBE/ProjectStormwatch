#pragma once

#include "CoreMinimal.h"
#include "IContentBrowserSingleton.h"
#include "PropertyCustomizationHelpers.h"
#include "ReacousticSoundAsset.h"
#include "ReacousticSoundAssetTypeActions.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Layout/SScrollBox.h"

class ReacousticAssociationEditorWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(ReacousticAssociationEditorWidget) {}
	SLATE_END_ARGS()
	TSharedPtr<SScrollBox> ScrollBoxWidget;

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

private:
	/** Adds information to the widget's list */
	void AddToList(const FString& Info);
	void AddActorToList(AActor* Actor, bool hascomponent,
						bool IsGenerateHitEventsSnabled,
						UReacousticSoundAsset* SoundAssociation);
};

class SActorListItem : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SActorListItem)
	{}

	// Actor to be displayed
	SLATE_ARGUMENT(AActor*, Actor)

	// Flag if the actor has the component
	SLATE_ARGUMENT(bool, HasComponent)

	// Flag if the actor generates hit events
	SLATE_ARGUMENT(bool, IsGenerateHitEventsEnabled)

	// Sound Association Asset
	SLATE_ARGUMENT(UReacousticSoundAsset*, SoundAssociation)

	SLATE_END_ARGS()

	// ...

	void Construct(const FArguments& InArgs)
	{
		// Save the passed arguments
		Actor = InArgs._Actor;
		HasComponent = InArgs._HasComponent;
		IsGenerateHitEventsEnabled = InArgs._IsGenerateHitEventsEnabled;
		SoundAssociation = InArgs._SoundAssociation;

		// Determine the background color
		if (HasComponent && IsGenerateHitEventsEnabled && SoundAssociation)
			{
			BackgroundColor = FLinearColor(0.f, 1.f, 0.f);  // Green color
			}
		else
		{
			BackgroundColor = FLinearColor(1.f, 0.64f, 0.f);  // Orange color
		}

		// Now use this BackgroundBrush to set the background of the widget...

		ChildSlot
[
	SNew(SBorder)
	.ColorAndOpacity(BackgroundColor)
	.Padding(10.0f) // some padding for better appearance
	.VAlign(VAlign_Center)
	.HAlign(HAlign_Fill)
	[
	SNew(SHorizontalBox) // Use a horizontal box to place items next to each other

	// 1. Actor Name Text
	+ SHorizontalBox::Slot()
	.FillWidth(0.3f) // adjust as needed
	.VAlign(VAlign_Center)
	[
		SNew(STextBlock)
		.Text(FText::FromString(Actor->GetName()))
	]

	// 2. Asset picker for UReacousticSoundAsset
	+ SHorizontalBox::Slot()
	.FillWidth(0.5f)
	.VAlign(VAlign_Center)
	//[
	//	SNew(SAssetDropTarget)
	//	.OnAssetDropped(this, &SActorListItem::OnDrop)
	//	[
	//		SNew(SButton)
	//		.Text(LOCTEXT("SelectSoundAsset", "Select Sound Asset"))
	//		.OnClicked(this, &SActorListItem::OnNavigation)
	//	]
	]

	// 3. CheckBox for toggling hit events
	//+ SHorizontalBox::Slot()
	//.FillWidth(0.2f) // adjust as needed
	//.VAlign(VAlign_Center)
	[
		SNew(SCheckBox)
		// Other checkbox bindings as you've commented out
	]
];


	}

	FReply OnOpenAssetPicker();
	void OnAss(const FAssetData& AssetData);
	void OnAssetDropped(const FAssetData& AssetData);

	// ... 

	private:
	FLinearColor BackgroundColor;
	AActor* Actor = nullptr;
	bool HasComponent = false;
	bool IsGenerateHitEventsEnabled = false;
	UReacousticSoundAsset* SoundAssociation = nullptr;
};









