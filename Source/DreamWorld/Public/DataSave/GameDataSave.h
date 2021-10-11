// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "GameFramework/SaveGame.h"
#include "GameDataSave.generated.h"

class UWorldDataSave;
class UPlayerDataSave;

/**
 * 游戏数据存取类
 */
UCLASS()
class UGameDataSave : public USaveGame
{
	GENERATED_BODY()

public:
	UGameDataSave();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FGameSaveData GameData;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<FString, FWorldBasicSaveData> WorldDatas;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<FString, FCharacterBasicSaveData> PlayerDatas;

public:
	void RefreshGameData();

	FGameSaveData& GetGameData() { return GameData; }

	void SetGameData(FGameSaveData InGameData) { GameData = InGameData; }

	TMap<FString, FWorldBasicSaveData>& GetWorldDatas() { return WorldDatas; }

	TMap<FString, FCharacterBasicSaveData>& GetPlayerDatas() { return PlayerDatas; }
};
