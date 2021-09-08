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
	FGameData GameData;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<FString, FWorldBasicData> WorldDatas;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<FString, FCharacterBasicData> PlayerDatas;

public:
	void RefreshGameData();

	FGameData& GetGameData() { return GameData; }

	void SetGameData(FGameData InGameData) { GameData = InGameData; }

	TMap<FString, FWorldBasicData>& GetWorldDatas() { return WorldDatas; }

	TMap<FString, FCharacterBasicData>& GetPlayerDatas() { return PlayerDatas; }
};
