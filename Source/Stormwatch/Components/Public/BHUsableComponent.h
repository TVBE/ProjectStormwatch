// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "BHInteractableComponent.h"
#include "BHUsableObjectInterface.h"

#include "BHUsableComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBeginUseSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndUseSignature);

UCLASS(NotBlueprintable, BlueprintType, ClassGroup = "Barrelhouse", Meta = (BlueprintSpawnableComponent))
class STORMWATCH_API UBHUsableComponent : public UBHInteractableComponent, public IBHUsableObject
{
	GENERATED_BODY()

public:
	UBHUsableComponent();

	//~ Begin BHUsableObject Interface.
	virtual bool Use_Implementation(const AActor* Instigator) override;
	virtual void Release_Implementation(const AActor* Instigator) override;
	virtual EBHUseType GetUseType_Implementation() const override;
	//~ End BHUsableObject Interface.
	
	UPROPERTY(BlueprintAssignable)
	FOnBeginUseSignature OnBeginUse;
	
	UPROPERTY(BlueprintAssignable)
	FOnEndUseSignature OnEndUse;

private:
	UPROPERTY(EditAnywhere)
	EBHUseType UseType = EBHUseType::SinglePress;
};
