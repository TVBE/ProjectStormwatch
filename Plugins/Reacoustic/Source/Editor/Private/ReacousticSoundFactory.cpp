// Copyright (c) 2022-present Nino Saglia. All Rights Reserved.

#include "ReacousticSoundFactory.h"
#include "ReacousticSoundAsset.h"

UReacousticSoundFactory::UReacousticSoundFactory()
{
	SupportedClass = UReacousticSoundAsset::StaticClass();
	bCreateNew = true;
}

UObject* UReacousticSoundFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags,
	UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UReacousticSoundAsset>(InParent, Class, Name, Flags, Context);
}