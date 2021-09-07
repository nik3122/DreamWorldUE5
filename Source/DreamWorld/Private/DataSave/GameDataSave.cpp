// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "DataSave/GameDataSave.h"
#include "DataSave/WorldDataSave.h"
#include "DataSave/PlayerDataSave.h"
#include "Kismet/GameplayStatics.h"

UGameDataSave::UGameDataSave()
{
	// set default pawn class to our Blueprinted character
}

void UGameDataSave::SaveGameData(const int32 InUserIndex)
{
	UGameplayStatics::SaveGameToSlot(this, TEXT("GameData"), InUserIndex);
}
