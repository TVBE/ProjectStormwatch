// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Desktop.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "Interaction", Meta = (DisplayName = "Desktop"))
class FROSTBITE_API ADesktop : public AActor
{
	GENERATED_BODY()

protected:
	/** The text displayed on the desktop screen. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Desktop|Screen", Meta = (DisplayName = "Cursor Screen Space"))
	UBoxComponent* ScreenSpace;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Desktop|Cursor", Meta = (DisplayName = "Cursor Mesh"))
	UStaticMeshComponent* CursorMesh;
	
	/** The text displayed on the desktop screen. */
	UPROPERTY(BlueprintReadWrite, Category = "Desktop|Text", Meta = (DisplayName = "Text Line Array"))
	TArray<FString> TextLineArray;

	/** The amount of lines that can be displayed on the desktop screen. */
	UPROPERTY(BlueprintReadOnly, Category = "Desktop|Text", Meta = (DisplayName = "Maximum Lines"))
	int32 MaxLines {18};

	/** The amount of characters allowed per line. */
	UPROPERTY(BlueprintReadOnly, Category = "Desktop|Text", Meta = (DisplayName = "Maximum Characters Per Line"))
	int32 MaxLineLength {72};

	/** The full text to display on the desktop's screen. */
	UPROPERTY(BlueprintReadOnly, Category = "Desktop|Text", Meta = (DisplayName = "Display Text"))
	FString DisplayText;
	
public:	
	ADesktop();

protected:
	virtual void BeginPlay() override;
	virtual void PostInitProperties() override;

	/** Moves the cursor over the screen. */
	UFUNCTION(BlueprintCallable, Category = "Desktop|Cursor", Meta = (DisplayName = "Move Cursor"))
	void MoveCursor(FVector InputVelocity);


	/** Formats the display text. */
	UFUNCTION(BlueprintCallable, Category = "Keyboard", Meta = (DisplayName = "Format Display Text"))
	void FormatDisplayText();
	
	/** Adds a character to the array of text lines. */
	UFUNCTION(BlueprintCallable, Category = "Keyboard", Meta = (DisplayName = "Add Character To Display List"))
	void AppendCharacterToArray(const FString& Character);

	/** Returns a reference to the last line available with space. */
	UFUNCTION(BlueprintCallable, Category = "Keyboard", Meta = (DisplayName = "Get Available Text Line From List"))
	UPARAM(ref) FString& GetLastLineWithSpace();
};
	
