// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "Components/SceneComponent.h"
#include "BHUsableObjectInterface.h"
#include "BHUseComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBeginUseDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndUseDelegate);

UCLASS(NotBlueprintable, BlueprintType, ClassGroup = "Interaction", Meta = (BlueprintSpawnableComponent))
class STORMWATCH_API UBHUseComponent : public USceneComponent, public IBHUsableObject
{
	GENERATED_BODY()

public:
	UBHUseComponent();

	//~ Begin BHUsableObject Interface.
	virtual bool BeginUse_Implementation(const AActor* Interactor) override;
	virtual bool EndUse_Implementation(const AActor* Interactor) override;
	virtual EBHUseType GetUseType_Implementation() const override {return UseType; }
	//~ End BHUsableObject Interface.
	
	UPROPERTY(BlueprintAssignable)
	FOnBeginUseDelegate OnBeginUse;
	
	UPROPERTY(BlueprintAssignable)
	FOnEndUseDelegate OnEndUse;

private:
	UPROPERTY(EditAnywhere)
	EBHUseType UseType = EBHUseType::SinglePress;
};
