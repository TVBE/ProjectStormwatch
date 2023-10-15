// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHHomeComputerKeyboard.h"

ABHKeyboard::ABHKeyboard()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABHKeyboard::BeginPlay()
{
	Super::BeginPlay();
}

FString ABHKeyboard::GetRandomCharacter()
{
	const FRandomStream RandomStream(FMath::Rand());
	const int32 RandomIndex = RandomStream.RandRange(0, 25);
	const TCHAR RandomChar = TCHAR('A' + RandomIndex);
	return FString(1, &RandomChar);
}


