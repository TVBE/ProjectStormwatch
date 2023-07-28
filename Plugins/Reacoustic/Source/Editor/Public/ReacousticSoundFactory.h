// Copyright (c) 2022-present Nino Saglia. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "ReacousticSoundFactory.generated.h"

UCLASS()
class REACOUSTICEDITOR_API UReacousticSoundFactory : public UFactory
{
	GENERATED_BODY()

public:
	
	//* A weak pointer to a USoundWave object. It doesn't prevent the USoundWave from being destroyed when there are no more strong references to it. */
	TObjectPtr<USoundWave> StagedSoundWave;

	
	UReacousticSoundFactory();

	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};