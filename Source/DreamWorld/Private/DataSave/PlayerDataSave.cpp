// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "DataSave/PlayerDataSave.h"
#include "Engine/World.h"
#include "World/Chunk.h"
#include "Character/Player/DWPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "World/WorldManager.h"
#include "World/Components/WorldTimerComponent.h"

UPlayerDataSave::UPlayerDataSave()
{
	// set default pawn class to our Blueprinted character
	
	PlayerData = FCharacterData();
}

void UPlayerDataSave::RefreshPlayerData()
{
	if(ADWPlayerCharacter* PlayerCharacter = UDWHelper::GetPlayerCharacter(this))
	{
		PlayerData = PlayerCharacter->ToData();
	}
}
