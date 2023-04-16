// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#include "Desktop.h"

#include "MeshInteractionComponent.h"
#include "Components/BoxComponent.h"

ADesktop::ADesktop()
{
	PrimaryActorTick.bCanEverTick = false;

	ScreenMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Screen"));
	RootComponent = ScreenMesh;

	ScreenSpace = CreateDefaultSubobject<UBoxComponent>(TEXT("Screen Space"));
	ScreenSpace->SetupAttachment(RootComponent);
	
	CursorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cursor"));
	CursorMesh->SetupAttachment(RootComponent);

	InteractionComponent = CreateDefaultSubobject<UMeshInteractionComponent>(TEXT("Interaction Component"));
}

bool ADesktop::BeginUse_Implementation(const AActor* Interactor)
{
	return IUsableObject::BeginUse_Implementation(Interactor);
}

bool ADesktop::EndUse_Implementation(const AActor* Interactor)
{
	return IUsableObject::EndUse_Implementation(Interactor);
}

void ADesktop::PostInitProperties()
{
    Super::PostInitProperties();
    TextLineArray.Init(FString(), MaxLines);
}

void ADesktop::BeginPlay()
{
	Super::BeginPlay();
}

void ADesktop::MoveCursor(FVector InputVelocity)
{
	FVector NewCursorPosition {CursorMesh->GetComponentLocation() + InputVelocity};
	
	const FVector ScreenSpaceMin {ScreenSpace->GetComponentLocation() - (ScreenSpace->GetScaledBoxExtent())};
	const FVector ScreenSpaceMax {ScreenSpace->GetComponentLocation() + (ScreenSpace->GetScaledBoxExtent())};

	NewCursorPosition.X = FMath::Clamp(NewCursorPosition.X, ScreenSpaceMin.X, ScreenSpaceMax.X);
	NewCursorPosition.Y = FMath::Clamp(NewCursorPosition.Y, ScreenSpaceMin.Y, ScreenSpaceMax.Y);
	NewCursorPosition.Z = FMath::Clamp(NewCursorPosition.Z, ScreenSpaceMin.Z, ScreenSpaceMax.Z);

	CursorMesh->SetWorldLocation(NewCursorPosition);
}

void ADesktop::FormatDisplayText()
{
	DisplayText.Reset();
	for (int i {0}; i < TextLineArray.Num(); ++i)
	{
		const FString& Line {TextLineArray[i]};
		DisplayText.Append(Line);
		if (i != TextLineArray.Num() - 1)
		{
			DisplayText.AppendChar('\n');
		}
	}
}

void ADesktop::AppendCharacterToArray(const FString& Character)
{
	if (TextLineArray.Num() == 0 || TextLineArray.Last().Len() >= MaxLineLength)
	{
		const FString NewLine;
		TextLineArray.Add(NewLine);
	}

	FString& LastLine {TextLineArray.Last()};
	LastLine.AppendChar(Character[0]);

	if (TextLineArray.Num() > MaxLines)
	{
		TextLineArray.RemoveAt(0);
	}
}

FString& ADesktop::GetLastLineWithSpace()
{
	if (TextLineArray.Num() == 0)
	{
		const FString NewLine;
		TextLineArray.Add(NewLine);
	}

	FString& LastLine {TextLineArray.Last()};
	if (LastLine.Len() >= MaxLineLength)
	{
		const FString NewLine;
		TextLineArray.Add(NewLine);
		LastLine = TextLineArray.Last();
	}
	return LastLine;
}





