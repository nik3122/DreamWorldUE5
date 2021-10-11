// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "GameFramework/SaveGame.h"
#include "PlayerDataSave.generated.h"

class ADWPlayerCharacter;

/**
 * 玩家数据存取类
 */
UCLASS()
class UPlayerDataSave : public USaveGame
{
	GENERATED_BODY()

public:
	UPlayerDataSave();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FCharacterSaveData PlayerData;

public:
	UFUNCTION(BlueprintCallable)
	void RefreshPlayerData();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FCharacterSaveData& GetPlayerData() { return PlayerData; }

	UFUNCTION(BlueprintCallable)
	void SetPlayerData(FCharacterSaveData InPlayerData) {PlayerData = InPlayerData;}
};
