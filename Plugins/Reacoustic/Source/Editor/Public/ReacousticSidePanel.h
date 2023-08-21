#pragma once

#include "ReacousticDataTypes.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Text/STextBlock.h"
#include "AssetThumbnail.h"
#include "Widgets/Docking/SDockTab.h"






class ReacousticSidePanel : public SDockTab
{
public:
	SLATE_BEGIN_ARGS(ReacousticSidePanel) {}
	SLATE_END_ARGS()
	

	struct SidePanelEntry
	{
		bool IsParent{false};
		FReacousticValidityResult Data{};
	};
	TArray<TSharedPtr<SidePanelEntry>> SidePanelArray;
	TArray<TSharedPtr<SidePanelEntry>> SidePanelParentArray;
	TSharedPtr<STreeView<TSharedPtr<SidePanelEntry>>> SidePanelTreeView;
	TSharedPtr<FAssetThumbnailPool> ThumbnailPool;



	void Construct(const FArguments& InArgs);

	void UpdateActorList(TArray<TSharedPtr<FReacousticValidityResult>> NewActorDataList);
	
	/** Generates a row for the MeshActorTreeView */
	TSharedRef<ITableRow> OnGenerateRowForMeshActorTree(TSharedPtr<SidePanelEntry> InItem, const TSharedRef<STableViewBase>& OwnerTable);

	/** Provides child items for a given item in the MeshActorTreeView */
	void OnGetChildrenForMeshActorTree(TSharedPtr<SidePanelEntry> InParent, TArray<TSharedPtr<SidePanelEntry>>& OutChildren);
	
	TSharedRef<ITableRow> OnGenerateRowForActorList(TSharedPtr<SidePanelEntry> Item, const TSharedRef<STableViewBase>& OwnerTable);
};