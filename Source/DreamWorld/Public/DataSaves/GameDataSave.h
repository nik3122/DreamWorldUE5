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
	int32 UserIndex;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<FString, FWorldData> WorldDatas;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<FString, FCharacterData> PlayerDatas;

public:
	bool IsExistWorldData(const FString& InWorldName);

	bool IsExistPlayerData(const FString& InPlayerName);

	void SaveGameData(const int32 InUserIndex = -1);

	void CreateWorldData(FWorldData InSaveData);

	void CreatePlayerData(FCharacterData InSaveData);

	void SaveWorldData(UWorldDataSave* InWorldDataSave);

	void SavePlayerData(UPlayerDataSave* InPlayerDataSave);

	void RemoveWorldData(const FString& InWorldName);
	
	void RemovePlayerData(const FString& InPlayerName);

	UWorldDataSave* LoadWorldData(const FString& InWorldName);

	UPlayerDataSave* LoadPlayerData(const FString& InPlayerName);

	TArray<FString> GetWorldNames();

	TArray<FString> GetPlayerNames();

	TArray<FWorldData> GetWorldDatas();

	TArray<FCharacterData> GetPlayerDatas();
	
	TArray<UWorldDataSave*> GetWorldDataSaves();

	TArray<UPlayerDataSave*> GetPlayerDataSaves();

	int32 GetUserIndex() const { return UserIndex; }

	void SetUserIndex(int32 InUserIndex) { UserIndex = InUserIndex; }
};
