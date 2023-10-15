// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "..\..\..\Interfaces\Public\BHUsableObjectInterface.h"
#include "GameFramework/Actor.h"
#include "BHHomeComputer.generated.h"

class UBHGrabComponent;
class UBoxComponent;
class UStaticMeshComponent;

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "HomeComputer")
class STORMWATCH_API ABHHomeComputer : public AActor, public IBHUsableObject
{
	GENERATED_BODY()

public:
	ABHHomeComputer();
	
	bool BeginUse_Implementation(const AActor* Interactor) override;
	bool EndUse_Implementation(const AActor* Interactor) override;
	EBHUseType GetUseType_Implementation() const override { return EBHUseType::PressAndHold; }

protected:
	virtual void BeginPlay() override;
	virtual void PostInitProperties() override;
	
	UFUNCTION(BlueprintCallable, Category = "Cursor")
	void MoveCursor(FVector2D InputVelocity);
	
	UFUNCTION(BlueprintCallable, Category = "Keyboard")
	void FormatDisplayText();
	
	UFUNCTION(BlueprintCallable, Category = "Keyboard", Meta = (DisplayName = "Add Character To Display List"))
	void AppendCharacterToArray(const FString& Character);
	
	UFUNCTION(BlueprintCallable, Category = "Keyboard", Meta = (DisplayName = "Get Available Text Line From List"))
	UPARAM(ref) FString& GetLastLineWithSpace();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Screen")
	UStaticMeshComponent* Mesh;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Screen")
	UBoxComponent* ScreenSpace;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Cursor")
	UStaticMeshComponent* CursorMesh;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UBHGrabComponent* GrabComponent;
	
	UPROPERTY(BlueprintReadWrite, Category = "Text")
	TArray<FString> TextLineArray;
	
	UPROPERTY(BlueprintReadOnly, Category = "Text", Meta = (DisplayName = "Maximum Lines On Screen"))
	int32 MaxLines = 18;
	
	UPROPERTY(BlueprintReadOnly, Category = "Text", Meta = (DisplayName = "Maximum Characters Per Line"))
	int32 MaxLineLength = 72;
	
	UPROPERTY(BlueprintReadOnly, Category = "Text")
	FString DisplayText;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction", Meta = (DisplayName = "Use Press And Hold Time"))
	float UseTime = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bTurnedOn = true;
	
	UPROPERTY(BlueprintReadWrite)
	bool bBroken = false;
};
	
