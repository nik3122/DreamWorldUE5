// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "DreamWorld.h"
#include "Engine/GameInstance.h"
#include "DWGameInstance.generated.h"

class UWidgetPanelBase;
class UWorldDataSave;
class UGameDataSave;
class UPlayerDataSave;

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
	UGameDataSave* GameDataSave;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Default")
	UWorldDataSave* WorldDataSave;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Default")
	UPlayerDataSave* PlayerDataSave;

public:
	void Init() override;
	
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
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<FString> GetWorldNames();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<FString> GetPlayerNames();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<FWorldData> GetWorldDatas();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<FCharacterData> GetPlayerDatas();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<UWorldDataSave*> GetWorldDataSaves();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<UPlayerDataSave*> GetPlayerDataSaves();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UGameDataSave* GetGameDataSave() const { return GameDataSave; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UWorldDataSave* GetWorldDataSave() const { return WorldDataSave; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UPlayerDataSave* GetPlayerDataSave() const { return PlayerDataSave; }
};
