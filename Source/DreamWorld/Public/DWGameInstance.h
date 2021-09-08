// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "DreamWorld.h"
#include "Engine/GameInstance.h"
#include "DWGameInstance.generated.h"

/**
 * 游戏实例基类
 */
UCLASS()
class UDWGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UDWGameInstance();

public:
	virtual void Init() override;

	//////////////////////////////////////////////////////////////////////////
	// GameData
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UGameDataSave* GameDataSave;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 UserIndex;

public:
	UFUNCTION(BlueprintCallable)
	void SaveGameData(bool bRefresh = true);

	UFUNCTION(BlueprintCallable)
	UGameDataSave* LoadGameData();
	
	UFUNCTION(BlueprintCallable)
	void CreateGameData(FGameData InGameData, bool bSaveData = true);

	UFUNCTION(BlueprintCallable)
	void DeleteGameData();

	UFUNCTION(BlueprintPure)
	FGameData GetGameData() const;

	UFUNCTION(BlueprintPure)
	int32 GetUserIndex() const { return UserIndex; }

	UFUNCTION(BlueprintCallable)
	void SetUserIndex(int32 InUserIndex) { UserIndex = InUserIndex; }

	//////////////////////////////////////////////////////////////////////////
	// WorldData
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<FString, class UWorldDataSave*> WorldDataSaves;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FString CurrentWorldName;

public:
	UFUNCTION(BlueprintCallable)
	bool IsExistWorldData(const FString& InWorldName);
	
	UFUNCTION(BlueprintCallable)
	void SaveWorldData(const FString& InWorldName, bool bRefresh = true);
		
	UFUNCTION(BlueprintCallable)
	class UWorldDataSave* LoadWorldData(const FString& InWorldName = TEXT(""));
	
	UFUNCTION(BlueprintCallable)
	void UnloadWorldData(const FString& InWorldName, bool bSaveData = true);

	UFUNCTION(BlueprintCallable)
	void CreateWorldData(FWorldData InWorldData, bool bSaveData = true);

	UFUNCTION(BlueprintCallable)
	void RemoveWorldData(const FString& InWorldName);

	UFUNCTION(BlueprintPure)
	TMap<FString, FWorldBasicData> GetWorldDatas() const;
	
	UFUNCTION(BlueprintPure)
	FString GetCurrentWorldName() const { return CurrentWorldName; }

	UFUNCTION(BlueprintCallable)
	void SetCurrentWorldName(const FString& InCurrentWorldName) { CurrentWorldName = InCurrentWorldName; }

	//////////////////////////////////////////////////////////////////////////
	// PlayerData
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<FString, class UPlayerDataSave*> PlayerDataSaves;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FString CurrentPlayerName;

public:
	UFUNCTION(BlueprintCallable)
	bool IsExistPlayerData(const FString& InPlayerName);
		
	UFUNCTION(BlueprintCallable)
	void SavePlayerData(const FString& InPlayerName, bool bRefresh = true);
	
	UFUNCTION(BlueprintCallable)
	class UPlayerDataSave* LoadPlayerData(const FString& InPlayerName = TEXT(""));
	
	UFUNCTION(BlueprintCallable)
	void UnloadPlayerData(const FString& InPlayerName, bool bSaveData = true);

	UFUNCTION(BlueprintCallable)
	void CreatePlayerData(FCharacterData InPlayerData, bool bSaveData = true);

	UFUNCTION(BlueprintCallable)
	void RemovePlayerData(const FString& InPlayerName);

	UFUNCTION(BlueprintPure)
	TMap<FString, FCharacterBasicData> GetPlayerDatas() const;
	
	UFUNCTION(BlueprintPure)
	FString GetCurrentPlayerName() const { return CurrentPlayerName; }

	UFUNCTION(BlueprintCallable)
	void SetCurrentPlayerName(const FString& InCurrentPlayerName) { CurrentPlayerName = InCurrentPlayerName; }
};
