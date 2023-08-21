#pragma once

#include "EdMode.h"
#include "EditorModeRegistry.h"
#include "ReacousticSidePanel.h"
#include "Widgets/Layout/SScrollBox.h"




class ReacousticEditorMode : public FEdMode
{
public:
	const static FEditorModeID EM_ReacousticEditorModeId;
	virtual void Enter() override;
	virtual void Exit() override;
	void OnPropertyChanged(UObject* Object, FPropertyChangedEvent& PropertyChangedEvent);
	void UpdateHighlightedActorsList();
	void UpdateActorListUI();


	virtual void Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI) override;


	void HighlightActorsInWorld();

private:
	TArray<AActor*> HighlightedActors;
	TSharedPtr<ReacousticSidePanel> SidePanelWidget;

};
