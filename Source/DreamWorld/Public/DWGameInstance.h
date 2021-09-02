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

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Default")
	class UGameDataSave* GameDataSave;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Default")
	class UWorldDataSave* WorldDataSave;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Default")
	class UPlayerDataSave* PlayerDataSave;

public:
	virtual void Init() override;
	
	UFUNCTION(BlueprintCallable)
	void SaveGameData();
	
	UFUNCTION(BlueprintCallable)
	void SaveWorldData();
		
	UFUNCTION(BlueprintCallable)
	void SavePlayerData();
	
	UFUNCTION(BlueprintCallable)
	bool IsExistWorldData(const FString& InWorldName);

	UFUNCTION(BlueprintCallable)
	bool IsExistPlayerData(const FString& InPlayerName);

	UFUNCTION(BlueprintCallable)
	void LoadGameData(const int32 InUserIndex = 0);
		
	UFUNCTION(BlueprintCallable)
	void LoadWorldData(const FString& InWorldName);
	
	UFUNCTION(BlueprintCallable)
	void LoadPlayerData(const FString& InPlayerName);
			
	UFUNCTION(BlueprintCallable)
	void UnloadWorldData(bool bData = true);
	
	UFUNCTION(BlueprintCallable)
	void UnloadPlayerData(bool bData = true);

	UFUNCTION(BlueprintCallable)
	void CreateWorldData(FWorldData InWorldData, bool bAutoLoad = true);

	UFUNCTION(BlueprintCallable)
	void CreatePlayerData(FCharacterData InPlayerData, bool bAutoLoad = true);

	UFUNCTION(BlueprintCallable)
	void RemoveWorldData(const FString& InWorldName);

	UFUNCTION(BlueprintCallable)
	void RemovePlayerData(const FString& InPlayerName);

public:
	UFUNCTION(BlueprintPure)
	TArray<FString> GetWorldNames();

	UFUNCTION(BlueprintPure)
	TArray<FString> GetPlayerNames();

	UFUNCTION(BlueprintPure)
	TArray<FWorldData> GetWorldDatas();

	UFUNCTION(BlueprintPure)
	TArray<FCharacterData> GetPlayerDatas();

	UFUNCTION(BlueprintPure)
	TArray<class UWorldDataSave*> GetWorldDataSaves();

	UFUNCTION(BlueprintPure)
	TArray<class UPlayerDataSave*> GetPlayerDataSaves();

	UFUNCTION(BlueprintPure)
	class UGameDataSave* GetGameDataSave() const { return GameDataSave; }

	UFUNCTION(BlueprintPure)
	class UWorldDataSave* GetWorldDataSave() const { return WorldDataSave; }

	UFUNCTION(BlueprintPure)
	class UPlayerDataSave* GetPlayerDataSave() const { return PlayerDataSave; }
};
