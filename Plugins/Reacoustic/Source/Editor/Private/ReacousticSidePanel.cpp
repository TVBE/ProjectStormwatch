#include "ReacousticSidePanel.h"

#include "SAssetView.h"
#include "AssetThumbnail.h"
#include "Serialization/JsonTypes.h"


void ReacousticSidePanel::Construct(const FArguments& InArgs)
{
	ThumbnailPool = MakeShareable(new FAssetThumbnailPool(100));

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(FText::FromString("Reacoustic Side Panel"))
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
		SAssignNew(SidePanelTreeView, STreeView<TSharedPtr<SidePanelEntry>>)
			.TreeItemsSource(&SidePanelParentArray)
			.OnGenerateRow(this, &ReacousticSidePanel::OnGenerateRowForMeshActorTree)
			.OnGetChildren(this, &ReacousticSidePanel::OnGetChildrenForMeshActorTree)
		]
	];
}

void ReacousticSidePanel::UpdateActorList(TArray<TSharedPtr<FReacousticValidityResult>> NewActorDataList)
{
	SidePanelArray.Empty();
	SidePanelParentArray.Empty();

	/** TSet ensures each entry is unique, it doesnt allow multiple entries of the same value by default.*/
	TSet<UStaticMesh*> UniqueMeshes;
	for (const auto& ActorData : NewActorDataList)
	{

		TSharedPtr<SidePanelEntry> SidePanelItem = MakeShared<SidePanelEntry>();
		SidePanelItem->Data = *ActorData;  
		SidePanelItem->IsParent = false;  
		SidePanelArray.Add(SidePanelItem);
		
		UStaticMesh* Mesh = ActorData->OwnerStaticMesh;
		if (Mesh)
		{
			UniqueMeshes.Add(Mesh);
		}
	}
	
	/** Create a FReacousticValidityResult for each unique mesh and add it to MeshArray. */
	for (UStaticMesh* Mesh : UniqueMeshes)
	{
		TSharedPtr<SidePanelEntry> MeshItem = MakeShared<SidePanelEntry>();
		MeshItem->Data.OwnerStaticMesh =Mesh;
		MeshItem->IsParent = true;
		SidePanelParentArray.Add(MeshItem);
	}

	SidePanelTreeView->RequestListRefresh();
}

/**Receives the actor data and sorts them by corresponding static meshes.*/
TSharedRef<ITableRow> ReacousticSidePanel::OnGenerateRowForMeshActorTree(TSharedPtr<SidePanelEntry> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	const float ThumbnailSize = 16.0f;

	/** Static meshes, parent entries */
	if (Item.IsValid() && Item->Data.OwnerStaticMesh && Item->IsParent)
	{
		FString MeshName = Item->Data.OwnerStaticMesh->GetName();
		TSharedPtr<FAssetThumbnail> Thumbnail = MakeShareable(new FAssetThumbnail(Item->Data.OwnerStaticMesh, ThumbnailSize, ThumbnailSize, ThumbnailPool));
        
		return SNew(STableRow<TSharedPtr<SidePanelEntry>>, OwnerTable)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SBox)
				.WidthOverride(ThumbnailSize)
				.HeightOverride(ThumbnailSize)
				[
					Thumbnail->MakeThumbnailWidget()
				]
			]
			+ SHorizontalBox::Slot()
			.Padding(5.0f, 0.0f, 0.0f, 0.0f)
			[
				SNew(STextBlock)
				.Visibility(EVisibility::Visible)
				.ColorAndOpacity(FColor::White)
				.Text(FText::FromString(MeshName))
			]
		];
	}
	/** Actors, children entries */
	else
	{
		auto DetermineColor = [&]() -> FColor
		{
			if (Item->Data.IsValid())
			{
				return Item->Data.OverrideAsset ? FColor::Cyan : FColor::Green;
			}
			return FColor::Orange;
		};

		FColor BackgroundColor = DetermineColor(); 

		FString Name = TEXT("Unknown");
		if (Item.IsValid() && Item->Data.OwnerStaticMesh)
		{
			Name = Item->Data.Name;
		}

		return SNew(STableRow<TSharedPtr<FReacousticValidityResult>>, OwnerTable)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(2.0f, 0.0f, 5.0f, 0.0f)
			[
				/** Background Color */
				SNew(SBorder)
				.BorderBackgroundColor(BackgroundColor)
				.Padding(FMargin(5.0f))  
				[
					SNew(STextBlock)
					.Visibility(EVisibility::Visible)
					.Text(FText::FromString(Name))
				]
			]
		];

	}
}



void ReacousticSidePanel::OnGetChildrenForMeshActorTree(TSharedPtr<SidePanelEntry> InParent, TArray<TSharedPtr<SidePanelEntry>>& OutChildren)
{
	if(InParent->IsParent)
	{
		UStaticMesh* ParentMesh = InParent->Data.OwnerStaticMesh;
		for (const auto& Child : SidePanelArray)
		{
			/** Check if the child is not the same as the parent and if the child's OwnerStaticMesh matches the ParentMesh. */
			if (Child != InParent && Child->Data.OwnerStaticMesh && Child->Data.OwnerStaticMesh == ParentMesh)
			{
				if(!Child->IsParent) 
				{
					OutChildren.Add(Child);
				}
			}
		}
	}
	SidePanelTreeView->SetItemExpansion(InParent, true);
}




