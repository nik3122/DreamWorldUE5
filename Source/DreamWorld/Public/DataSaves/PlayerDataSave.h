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
	FCharacterData PlayerData;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<FString, FWorldRecordData> WorldRecordDatas;

public:
	UFUNCTION(BlueprintCallable)
	void SavePlayerData(AWorldManager* InWorldManager, ADWPlayerCharacter* InPlayerCharacter);
	
	UFUNCTION(BlueprintCallable)
	bool IsExistWorldRecord(const FString& InWorldName);

	UFUNCTION(BlueprintCallable)
	FWorldRecordData LoadWorldRecord(const FString& InWorldName);

	UFUNCTION(BlueprintCallable)
	void RemoveWorldRecord(const FString& InWorldName);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FCharacterData GetPlayerData() const { return PlayerData; }

	UFUNCTION(BlueprintCallable)
	void SetPlayerData(FCharacterData InPlayerData) {PlayerData = InPlayerData;}
};
