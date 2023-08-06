// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#include "AssetTypeActions_AmbiverseElement.h"
#include "AmbiverseElementAsset.h"
#include "AmbiverseEditor.h"
#include "Sound/SoundBase.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

FText FAssetTypeActions_AmbiverseElement::GetName() const
{
	return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_AmbiverseElement", "Ambiverse Element");
}

UClass* FAssetTypeActions_AmbiverseElement::GetSupportedClass() const
{
	return UAmbiverseElementAsset::StaticClass();
}

FColor FAssetTypeActions_AmbiverseElement::GetTypeColor() const
{
	return AMBIVERSE_ASSET_COLOR;
}

uint32 FAssetTypeActions_AmbiverseElement::GetCategories()
{
	return AMBIVERSE_ASSET_CATEGORY;
}

const TArray<FText>& FAssetTypeActions_AmbiverseElement::GetSubMenus() const
{
	static const TArray<FText> SubMenus
	{
		AMBIVERSE_ASSET_SUBMENUS
	};

	return SubMenus;
}

TSharedPtr<SWidget> FAssetTypeActions_AmbiverseElement::GetThumbnailOverlay(const FAssetData& AssetData) const
{
	auto OnGetDisplayBrushLambda = [this, AssetData]() -> const FSlateBrush*
	{
		if (IsElementPlaying(AssetData))
		{
			return FAppStyle::GetBrush("MediaAsset.AssetActions.Stop.Large");
		}

		return FAppStyle::GetBrush("MediaAsset.AssetActions.Play.Large");
	};

	auto OnClickedLambda = [this, AssetData]() -> FReply
	{
		if (IsElementPlaying(AssetData))
		{
			StopElement();
		}
		else
		{
			if (UAmbiverseElementAsset* ElementAsset {Cast<UAmbiverseElementAsset>(AssetData.GetAsset())})
			{
				PlayElement(TWeakObjectPtr<UAmbiverseElementAsset>(ElementAsset));
			}
		}
		return FReply::Handled();
	};

	auto OnToolTipTextLambda = [this, AssetData]() -> FText
	{
		if (IsElementPlaying(AssetData))
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
		if (Box.IsValid() && (Box->IsHovered() || IsElementPlaying(AssetData)))
		{
			return EVisibility::Visible;
		}

		return EVisibility::Hidden;
	};

	TSharedPtr<SButton> Widget;
	SAssignNew(Widget, SButton)
		.ButtonStyle(FAppStyle::Get(), "HoverHintOnly")
		.ToolTipText_Lambda(OnToolTipTextLambda)
		.Cursor(EMouseCursor::Default)
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

void FAssetTypeActions_AmbiverseElement::PlayElement(TWeakObjectPtr<UAmbiverseElementAsset> Object) const
{
	if (Object.IsValid())
	{
		if (UAmbiverseElementAsset* ElementAsset {Object.Get()})
		{
			if (USoundBase* Sound {ElementAsset->GetSound()})
			{
				GEditor->PlayPreviewSound(Sound);
			}
		}
	}
	else
	{
		StopElement();
	}
}

void FAssetTypeActions_AmbiverseElement::StopElement() const
{
	GEditor->ResetPreviewAudioComponent();
}

bool FAssetTypeActions_AmbiverseElement::IsElementPlaying(const FAssetData& AssetData) const
{
	const UAudioComponent* PreviewComponent {GEditor->GetPreviewAudioComponent()};
	if (PreviewComponent && PreviewComponent->Sound && PreviewComponent->IsPlaying())
	{
		if (UAmbiverseElementAsset* ElementAsset {Cast<UAmbiverseElementAsset>(AssetData.GetAsset())})
		{
			if (USoundBase* Sound {ElementAsset->GetSound()})
			{
				if (PreviewComponent->Sound == Sound)
				{
					return true;
				}
			}
		}
	}

	return false;
}
