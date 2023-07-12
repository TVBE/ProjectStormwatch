// Copyright (c) 2022-present Tim Verberne
// This source code is part of the Adaptive Ambience System plugin.

#include "AmbiverseElementAssetTypeActions.h"
#include "AmbiverseElementAsset.h"
#include "AmbiverseEditor.h"
#include "Sound/SoundBase.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

FText FAmbiverseElementAssetTypeActions::GetName() const
{
	return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_AmbiverseElement", "Ambiverse Element");
}

UClass* FAmbiverseElementAssetTypeActions::GetSupportedClass() const
{
	return UAmbiverseElementAsset::StaticClass();
}

FColor FAmbiverseElementAssetTypeActions::GetTypeColor() const
{
	return AMBIVERSE_ASSET_COLOR;
}

uint32 FAmbiverseElementAssetTypeActions::GetCategories()
{
	return AMBIVERSE_ASSET_CATEGORY;
}

const TArray<FText>& FAmbiverseElementAssetTypeActions::GetSubMenus() const
{
	static const TArray<FText> SubMenus
	{
		AMBIVERSE_ASSET_SUBMENUS
	};

	return SubMenus;
}

void FAmbiverseElementAssetTypeActions::PlaySound(USoundBase* Sound) const
{
	if ( Sound )
	{
		GEditor->PlayPreviewSound(Sound);
	}
	else
	{
		StopSound();
	}
}

void FAmbiverseElementAssetTypeActions::StopSound() const
{
	GEditor->ResetPreviewAudioComponent();
}

bool FAmbiverseElementAssetTypeActions::IsSoundPlaying(USoundBase* Sound) const
{
	const UAudioComponent* PreviewComp {GEditor->GetPreviewAudioComponent()};
	return PreviewComp && PreviewComp->Sound == Sound && PreviewComp->IsPlaying();
}

bool FAmbiverseElementAssetTypeActions::IsSoundPlaying(const FAssetData& AssetData) const
{
	const UAudioComponent* PreviewComp {GEditor->GetPreviewAudioComponent()};
	if (PreviewComp && PreviewComp->Sound && PreviewComp->IsPlaying())
	{
		if (PreviewComp->Sound->GetFName() == AssetData.AssetName)
		{
			if (PreviewComp->Sound->GetOutermost()->GetFName() == AssetData.PackageName)
			{
				return true;
			}
		}
	}

	return false;
}

void FAmbiverseElementAssetTypeActions::ExecutePlaySound(TArray<TWeakObjectPtr<USoundBase>> Objects) const
{
	for (auto ObjIt = Objects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		if (USoundBase* Sound {(*ObjIt).Get()})
		{
			PlaySound(Sound);
			break;
		}
	}
}

void FAmbiverseElementAssetTypeActions::ExecuteStopSound(TArray<TWeakObjectPtr<USoundBase>> Objects) const
{
	StopSound();
}

TSharedPtr<SWidget> FAmbiverseElementAssetTypeActions::GetThumbnailOverlay(const FAssetData& AssetData) const
{
	auto OnGetDisplayBrushLambda = [this, AssetData]() -> const FSlateBrush*
	{
		
		if (IsSoundPlaying(AssetData))
		{
			return FAppStyle::GetBrush("MediaAsset.AssetActions.Stop.Large");
		}

		return FAppStyle::GetBrush("MediaAsset.AssetActions.Play.Large");
	};

	auto OnClickedLambda = [this, AssetData]() -> FReply
	{
		if (IsSoundPlaying(AssetData))
		{
			StopSound();
		}
		else
		{
			PlaySound(Cast<USoundBase>(AssetData.GetAsset()));
		}
		return FReply::Handled();
	};

	auto OnToolTipTextLambda = [this, AssetData]() -> FText
	{
		if (IsSoundPlaying(AssetData))
		{
			return LOCTEXT("Thumbnail_StopSoundToolTip", "Stop selected sound");
		}

		return LOCTEXT("Thumbnail_PlaySoundToolTip", "Play selected sound");
	};

	TSharedPtr<SBox> Box;
	SAssignNew(Box, SBox)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.Padding(FMargin(2));

	auto OnGetVisibilityLambda = [this, Box, AssetData]() -> EVisibility
	{
		if (Box.IsValid() && (Box->IsHovered() || IsSoundPlaying(AssetData)))
		{
			return EVisibility::Visible;
		}

		return EVisibility::Hidden;
	};

	TSharedPtr<SButton> Widget;
	SAssignNew(Widget, SButton)
		.ButtonStyle(FAppStyle::Get(), "HoverHintOnly")
		.ToolTipText_Lambda(OnToolTipTextLambda)
		.Cursor(EMouseCursor::Default) // The outer widget can specify a DragHand cursor, so we need to override that here
		.ForegroundColor(FSlateColor::UseForeground())
		.IsFocusable(false)
		.OnClicked_Lambda(OnClickedLambda)
		.Visibility_Lambda(OnGetVisibilityLambda)
		[
			SNew(SImage)
			.Image_Lambda(OnGetDisplayBrushLambda)
		];

	Box->SetContent(Widget.ToSharedRef());
	Box->SetVisibility(EVisibility::Visible);

	return Box;
}

FReply FAmbiverseElementAssetTypeActions::OnThumbnailOverlayClicked() const
{
	UE_LOG(LogTemp, Error, TEXT("The button was clicked"))
	return FReply::Handled();
}

