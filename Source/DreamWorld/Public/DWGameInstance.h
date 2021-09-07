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
	void SaveGameData();

	UFUNCTION(BlueprintCallable)
	UGameDataSave* LoadGameData();

	//////////////////////////////////////////////////////////////////////////
	// WorldData
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<FString, class UWorldDataSave*> WorldDataSaves;

public:
	UFUNCTION(BlueprintCallable)
	bool IsExistWorldData(const FString& InWorldName);
	
	UFUNCTION(BlueprintCallable)
	void SaveWorldData(const FString& InWorldName);
		
	UFUNCTION(BlueprintCallable)
	class UWorldDataSave* LoadWorldData(const FString& InWorldName);
			
	UFUNCTION(BlueprintCallable)
	void UnloadWorldData(const FString& InWorldName, bool bSaveData = true);

	UFUNCTION(BlueprintCallable)
	void CreateWorldData(FWorldData InWorldData, bool bSaveData = true);

	UFUNCTION(BlueprintCallable)
	void RemoveWorldData(const FString& InWorldName);

	UFUNCTION(BlueprintPure)
	TArray<FString> GetWorldNames() const;

	UFUNCTION(BlueprintPure)
	TArray<FWorldData> GetWorldDatas() const;

	UFUNCTION(BlueprintPure)
	TArray<class UWorldDataSave*> GetWorldDataSaves() const;

	//////////////////////////////////////////////////////////////////////////
	// PlayerData
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<FString, class UPlayerDataSave*> PlayerDataSaves;

public:
	UFUNCTION(BlueprintCallable)
	bool IsExistPlayerData(const FString& InPlayerName);
		
	UFUNCTION(BlueprintCallable)
	void SavePlayerData(const FString& InPlayerName);
	
	UFUNCTION(BlueprintCallable)
	class UPlayerDataSave* LoadPlayerData(const FString& InPlayerName);
	
	UFUNCTION(BlueprintCallable)
	void UnloadPlayerData(const FString& InPlayerName, bool bSaveData = true);

	UFUNCTION(BlueprintCallable)
	void CreatePlayerData(FCharacterData InPlayerData, bool bSaveData = true);

	UFUNCTION(BlueprintCallable)
	void RemovePlayerData(const FString& InPlayerName);

	UFUNCTION(BlueprintPure)
	TArray<FString> GetPlayerNames() const;

	UFUNCTION(BlueprintPure)
	TArray<FCharacterData> GetPlayerDatas() const;

	UFUNCTION(BlueprintPure)
	TArray<class UPlayerDataSave*> GetPlayerDataSaves() const;
};
