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
	TArray<FString> WorldNames;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FString> PlayerNames;

public:
	void SaveGameData(const int32 InUserIndex = 0);

	TArray<FString> GetWorldNames() const { return WorldNames; }

	TArray<FString> GetPlayerNames() const { return PlayerNames; }
};
