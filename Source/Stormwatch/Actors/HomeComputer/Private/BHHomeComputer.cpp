// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHHomeComputer.h"

#include "..\..\..\Components\Public\BHGrabComponent.h"
#include "Components/BoxComponent.h"

ABHDesktop::ABHDesktop()
{
	PrimaryActorTick.bCanEverTick = false;

	ScreenMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Screen"));
	RootComponent = ScreenMesh;

	ScreenSpace = CreateDefaultSubobject<UBoxComponent>(TEXT("Screen Space"));
	ScreenSpace->SetupAttachment(RootComponent);
	
	CursorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cursor"));
	CursorMesh->SetupAttachment(RootComponent);

	GrabComponent = CreateDefaultSubobject<UBHGrabComponent>(TEXT("Grab Component"));
	GrabComponent->SetupAttachment(RootComponent);
}

bool ABHDesktop::BeginUse_Implementation(const AActor* Interactor)
{
	return IUsableObject::BeginUse_Implementation(Interactor);
}

bool ABHDesktop::EndUse_Implementation(const AActor* Interactor)
{
	return IUsableObject::EndUse_Implementation(Interactor);
}

void ABHDesktop::PostInitProperties()
{
    Super::PostInitProperties();
    TextLineArray.Init(FString(), MaxLines);
}

void ABHDesktop::BeginPlay()
{
	Super::BeginPlay();
}

void ABHDesktop::MoveCursor(FVector2D InputVelocity)
{
	FVector NewCursorPosition = CursorMesh->GetComponentLocation() + FVector(InputVelocity.X, InputVelocity.Y , 0);
	
	const FVector ScreenSpaceMin = ScreenSpace->GetComponentLocation() - (ScreenSpace->GetScaledBoxExtent());
	const FVector ScreenSpaceMax = ScreenSpace->GetComponentLocation() + (ScreenSpace->GetScaledBoxExtent());

	NewCursorPosition.X = FMath::Clamp(NewCursorPosition.X, ScreenSpaceMin.X, ScreenSpaceMax.X);
	NewCursorPosition.Y = FMath::Clamp(NewCursorPosition.Y, ScreenSpaceMin.Y, ScreenSpaceMax.Y);

	CursorMesh->SetWorldLocation(NewCursorPosition);
}

void ABHDesktop::FormatDisplayText()
{
	DisplayText.Reset();
	for (int i = 0; i < TextLineArray.Num(); ++i)
	{
		const FString& Line = TextLineArray[i];
		DisplayText.Append(Line);
		if (i != TextLineArray.Num() - 1)
		{
			DisplayText.AppendChar('\n');
		}
	}
}

void ABHDesktop::AppendCharacterToArray(const FString& Character)
{
	if (TextLineArray.Num() == 0 || TextLineArray.Last().Len() >= MaxLineLength)
	{
		const FString NewLine;
		TextLineArray.Add(NewLine);
	}

	FString& LastLine = TextLineArray.Last();
	LastLine.AppendChar(Character[0]);

	if (TextLineArray.Num() > MaxLines)
	{
		TextLineArray.RemoveAt(0);
	}
}

FString& ABHDesktop::GetLastLineWithSpace()
{
	if (TextLineArray.Num() == 0)
	{
		const FString NewLine;
		TextLineArray.Add(NewLine);
	}

	FString& LastLine = TextLineArray.Last();
	if (LastLine.Len() >= MaxLineLength)
	{
		const FString NewLine;
		TextLineArray.Add(NewLine);
		LastLine = TextLineArray.Last();
	}
	return LastLine;
}




