// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "UsableObjectInterface.h"
#include "GameFramework/Actor.h"
#include "Desktop.generated.h"

class UMeshInteractionComponent;
class UBoxComponent;
class UStaticMeshComponent;

/** Base class for an all-in-one desktop that can receive keyboard and mouse input.
 *	This class is expected to be extended and fully implemented in a blueprint derived class. */
UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "Interaction", Meta = (DisplayName = "Desktop"))
class FROSTBITE_API ADesktop : public AActor, public IUsableObject
{
	GENERATED_BODY()

protected:
	/** The static mesh of the desktop screen. */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Desktop|Screen", Meta = (DisplayName = "Screen Mesh"))
	UStaticMeshComponent* ScreenMesh;

	/** The text displayed on the desktop screen. */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Desktop|Screen", Meta = (DisplayName = "Cursor Screen Space"))
	UBoxComponent* ScreenSpace;
	
	/** The static mesh for the screen cursor*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Desktop|Cursor", Meta = (DisplayName = "Cursor Mesh"))
	UStaticMeshComponent* CursorMesh;

	/** The mesh interaction component for this actor. */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Desktop", Meta = (DisplayName = "Mesh Interaction Component"))
	UMeshInteractionComponent* InteractionComponent;
	
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

	/** The time the player requires to hold the use key down to turn on or shut off the computer. */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Desktop|Interaction", Meta = (DisplayName = "Use Press And Hold Time"))
	float UseTime {1.0f};

	/** If true, the desktop is currently turned on. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Desktop", Meta = (DisplayName = "Is Turned On"))
	bool IsTurnedOn {true};

	/** If true, the desktop is currently destroyed. */
	UPROPERTY(BlueprintReadWrite, Category = "Desktop", Meta = (DisplayName = "Is Broken"))
	bool IsBroken {false};
	
public:	
	ADesktop();

	bool BeginUse_Implementation(const AActor* Interactor) override;
	bool EndUse_Implementation(const AActor* Interactor) override;
	FORCEINLINE EUseType GetUseType_Implementation() const override { return EUseType::PressAndHold; }

protected:
	virtual void BeginPlay() override;
	virtual void PostInitProperties() override;

	/** Moves the cursor over the screen. */
	UFUNCTION(BlueprintCallable, Category = "Desktop|Cursor", Meta = (DisplayName = "Move Cursor"))
	void MoveCursor(FVector InputVelocity);
	
	/** Formats the display text. */
	UFUNCTION(BlueprintCallable, Category = "Desktop|Keyboard", Meta = (DisplayName = "Format Display Text"))
	void FormatDisplayText();
	
	/** Adds a character to the array of text lines. */
	UFUNCTION(BlueprintCallable, Category = "Desktop|Keyboard", Meta = (DisplayName = "Add Character To Display List"))
	void AppendCharacterToArray(const FString& Character);

	/** Returns a reference to the last line available with space. */
	UFUNCTION(BlueprintCallable, Category = "Desktop|Keyboard", Meta = (DisplayName = "Get Available Text Line From List"))
	UPARAM(ref) FString& GetLastLineWithSpace();
};
	
