// Copyright (c) 2022-present Nino Saglia. All Rights Reserved.

#include "ReacousticSoundAssetTypeActions.h"
#include "ReacousticSoundAsset.h"
#include "ReacousticEditor.h"
#include "ReacousticSoundFactory.h"
#include "Algo/AnyOf.h"
#include "ToolMenus.h"
#include "ContentBrowserMenuContexts.h"
#include "ReacousticSubsystem.h"
#include "Misc/PackageName.h"
#include "Sound/SoundWaveProcedural.h"
#include "ToolMenu.h"
#include "ToolMenuSection.h"
#include "Sound/SoundBase.h"


#define LOCTEXT_NAMESPACE "AssetTypeActions"

FText FReacousticSoundAssetTypeActions::GetName() const
{
	return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_ReacousticSound", "Reacoustic Sound");
}

UClass* FReacousticSoundAssetTypeActions::GetSupportedClass() const
{
	return UReacousticSoundAsset::StaticClass();
}

FColor FReacousticSoundAssetTypeActions::GetTypeColor() const
{
	return REACOUSTIC_ASSET_COLOR;
}

uint32 FReacousticSoundAssetTypeActions::GetCategories()
{
	return EAssetTypeCategories::Sounds;
}

const TArray<FText>& FReacousticSoundAssetTypeActions::GetSubMenus() const
{
	static const TArray<FText> SubMenus
	{
		LOCTEXT("AssetReacousticSubMenu", "Reacoustic")
	};

	return SubMenus;
}


void FReacousticSoundAssetTypeActions::PlaySound(USoundBase* Sound) const
{
	if ( Sound )
	{
		//GEditor->PlayPreviewSound();
	}
	else
	{
		StopSound();
	}
}

void FReacousticSoundAssetTypeActions::StopSound() const
{
	GEditor->ResetPreviewAudioComponent();
}

bool FReacousticSoundAssetTypeActions::IsSoundPlaying(USoundBase* Sound) const
{
	const UAudioComponent* PreviewComp {GEditor->GetPreviewAudioComponent()};
	return PreviewComp && PreviewComp->Sound == Sound && PreviewComp->IsPlaying();
}

bool FReacousticSoundAssetTypeActions::IsSoundPlaying(const FAssetData& AssetData) const
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

void FReacousticSoundAssetTypeActions::ExecutePlaySound(TArray<TWeakObjectPtr<USoundBase>> Objects) const
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

void FReacousticSoundAssetTypeActions::ExecuteStopSound(TArray<TWeakObjectPtr<USoundBase>> Objects) const
{
	StopSound();
}

TSharedPtr<SWidget> FReacousticSoundAssetTypeActions::GetThumbnailOverlay(const FAssetData& AssetData) const
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
			UReacousticSubsystem* ReacousticSubsystem = GEditor->GetEditorWorldContext().World()->GetSubsystem<UReacousticSubsystem>();

			UObject* SoundObject = StaticLoadObject(UObject::StaticClass(), nullptr, TEXT("/Reacoustic/Audio/Metasounds/MSS_Reacoustic")); //Asset path referencing for now. I might include Metasound plugin later.
			USoundBase* SoundBase = Cast<USoundBase>(SoundObject);
    
			if (UReacousticSoundAsset* CastedAsset = Cast<UReacousticSoundAsset>(AssetData.GetAsset()))
			{
				UWorld* World = GEditor->GetEditorWorldContext().World();
				UAudioComponent* AudioComponent = NewObject<UAudioComponent>(World);

				// Set the sound object as the sound base
				AudioComponent->SetSound(SoundBase);

				FString soundName = CastedAsset->Sound->GetFName().ToString();
				float ImpactValue = FMath::RandRange(0.0f,1.0f); //For now a random impact value is fun. Another option is retriggering the sound with a delay to showcase the full range of interactions a reacoustic sound has.
            
				FVector2d TimestampAndVolume = ReacousticSubsystem->GetTimeStampWithStrenght(CastedAsset, ImpactValue);
				float Timestamp = TimestampAndVolume.X;
				float Volume = TimestampAndVolume.Y;
				
				
				// Set parameters for the audio component

				AudioComponent->bAutoDestroy = false;
				AudioComponent->bIsUISound = true;
				AudioComponent->bAllowSpatialization = false;
				AudioComponent->bReverb = false;
				AudioComponent->bCenterChannelOnly = false;
				AudioComponent->bPreviewComponent = true;
				
				AudioComponent->SetFloatParameter(TEXT("Obj_StartTime"), Timestamp);
				AudioComponent->SetFloatParameter(TEXT("Obj_Velocity"), ImpactValue);
				AudioComponent->SetObjectParameter(TEXT("Obj_WaveAsset"), CastedAsset->Sound);
				AudioComponent->SetFloatParameter(TEXT("Obj_Length"), CastedAsset->ImpulseLength);
				

				
				AudioComponent->Play();


			}
			
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

FReply FReacousticSoundAssetTypeActions::OnThumbnailOverlayClicked() const
{
	UE_LOG(LogTemp, Error, TEXT("The button was clicked"))
	return FReply::Handled();
}





void FReacousticContentBrowserMenuExtension::RegisterMenus()
{
	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("ContentBrowser.AssetContextMenu.SoundWave");
	FToolMenuSection& Section = Menu->FindOrAddSection("GetAssetActions");

	Section.AddDynamicEntry("SoundWaveAssetConversion_CreateReacousticSound", FNewToolMenuSectionDelegate::CreateLambda([](FToolMenuSection& InSection)
	{
		if (const UContentBrowserAssetContextMenuContext* Context = InSection.FindContext<UContentBrowserAssetContextMenuContext>())
		{
			if (!Algo::AnyOf(Context->SelectedAssets, [](const FAssetData& AssetData){ return AssetData.IsInstanceOf<USoundWaveProcedural>(); }))
			{
				const TAttribute<FText> Label = LOCTEXT("SoundWave_CreateReacousticSound", "Create Reacoustic Sound");
				const TAttribute<FText> ToolTip = LOCTEXT("SoundWave_CreateReacousticSoundTooltip", "Creates an reacoustic sound asset using the selected sound wave.");
				const FSlateIcon Icon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "ClassIcon.SoundWave");
				const FToolMenuExecuteAction UIAction = FToolMenuExecuteAction::CreateStatic(&FReacousticContentBrowserMenuExtension::ExecuteCreateReacousticSound);

				InSection.AddMenuEntry("SoundWave_CreateReacousticSound", Label, ToolTip, Icon, UIAction);
			}
		}
	}));
}

void FReacousticContentBrowserMenuExtension::ExecuteCreateReacousticSound(const FToolMenuContext& MenuContext)
{
	const FString DefaultSuffix = TEXT("");
	FAssetToolsModule& AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>("AssetTools");

	/** Create the factory used to generate the asset */
	UReacousticSoundFactory* Factory = NewObject<UReacousticSoundFactory>();
	
	/** only converts 0th selected object for now (see return statement) */
	if (const UContentBrowserAssetContextMenuContext* Context = UContentBrowserAssetContextMenuContext::FindContextWithAssets(MenuContext))
	{
		for (USoundWave* Wave : Context->LoadSelectedObjectsIf<USoundWave>([](const FAssetData& AssetData) { return !AssetData.IsInstanceOf<USoundWaveProcedural>(); }))
		{
			Factory->StagedSoundWave = Wave; //* WeakPtr gets reset by the Factory after it is consumed */

			/** Determine an appropriate name */
			FString Prefix = TEXT("RS_");
			FString OriginalAssetName = Wave->GetOutermost()->GetName();
			
			/** Split the original asset name from its path */
			int32 LastSlashIndex;
			OriginalAssetName.FindLastChar('/', LastSlashIndex);
			FString Path = OriginalAssetName.Left(LastSlashIndex + 1);
			FString AssetName = OriginalAssetName.Mid(LastSlashIndex + 1);

			/** Add the prefix to the asset name only, not the full path */
			FString PrefixedAssetName = Path + Prefix + AssetName;
			
			FString Name;
			FString PackagePath;
			AssetToolsModule.Get().CreateUniqueAssetName(PrefixedAssetName, DefaultSuffix, PackagePath, Name);

			/** create new asset */
			AssetToolsModule.Get().CreateAsset(Name, FPackageName::GetLongPackagePath(PackagePath), UReacousticSoundAsset::StaticClass(), Factory);
		}
	}
}



