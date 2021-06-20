// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DWHelper.generated.h"

class UWidgetLoadingPanel;
class UDataTable;
class UWidgetPrimaryPanel;
class UWidgetMainMenu;
class UWidgetPausingMenu;
class WidgetLoadingPanel;
class UWidgetSettingPanel;
class UWidgetRoleChoosingPanel;
class UWidgetRoleCreatingPanel;
class UWidgetInventoryPanel;
class UWidgetWorldChoosingPanel;
class UWidgetWorldCreatingPanel;
class UWidgetInventoryBar;
class UWidgetInventoryBox;
class UGameDataSave;
class UWorldDataSave;
class UPlayerDataSave;

UENUM(BlueprintType)
enum class EDebugType : uint8
{
	Screen,
	Console,
	Both
};

UCLASS()
class DREAMWORLD_API UDWHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

private:
	static UWorld* World;

	static AWorldManager* WorldManager;

	static ADWGameState* GameState;

	static ADWGameMode* GameMode;

	static UDWGameInstance* GameInstance;

	static ADWPlayerCharacterController* PlayerController;

	static TSubclassOf<UWidgetVitalityHP> WidgetVitalityHPClass;

	static TSubclassOf<UWidgetCharacterHP> WidgetCharacterHPClass;

	static TSubclassOf<UWidgetWorldText> WidgetWorldTextClass;

	static TMap<FName, FItemData> ItemDatas;

	static TMap<FName, FVoxelData> VoxelDatas;

	static TMap<FName, FEquipData> EquipDatas;

	static TMap<FName, FEquipWeaponData> WeaponDatas;

	static TMap<FName, FEquipShieldData> ShieldDatas;

	static TMap<FName, FEquipArmorData> ArmorDatas;

	static TMap<FName, FPropData> PropDatas;
		
	static TMap<FName, FVitalityRaceData> VitalityRaceDatas;

	static TMap<FName, FCharacterRaceData> CharacterRaceDatas;

public:
	UFUNCTION(BlueprintCallable, Category = "DWHelper")
	static void Debug(const FString& Message, EDebugType DebugType = EDebugType::Screen, float Duration = 1.5f);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static EDirection InvertDirection(EDirection InDirection);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static FVector DirectionToVector(EDirection InDirection, FRotator InRotation = FRotator::ZeroRotator);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static FIndex DirectionToIndex(EDirection InDirection, FRotator InRotation = FRotator::ZeroRotator);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static FIndex GetAdjacentIndex(FIndex InIndex, EDirection InDirection, FRotator InRotation = FRotator::ZeroRotator);

public:
	static void Initialize(UWorld* InWorld);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static AWorldManager* GetWorldManager();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static ADWGameState* GetGameState();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static ADWGameMode* GetGameMode();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static UDWGameInstance* GetGameInstance();
		
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static ADWPlayerCharacter* GetPlayerCharacter();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static ADWPlayerCharacterController* GetPlayerController();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static UGameDataSave* GetGameDataSave();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static UWorldDataSave* GetWorldDataSave();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static UPlayerDataSave* GetPlayerDataSave();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static UWidgetPrimaryPanel* GetWidgetPrimaryPanel();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static UWidgetMainMenu* GetWidgetMainMenu();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static UWidgetPausingMenu* GetWidgetPauseMenu();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static UWidgetLoadingPanel* GetWidgetLoadingPanel();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static UWidgetSettingPanel* GetWidgetSettingPanel();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static UWidgetRoleChoosingPanel* GetWidgetRoleChoosingPanel();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static UWidgetRoleCreatingPanel* GetWidgetRoleCreatingPanel();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static UWidgetWorldChoosingPanel* GetWidgetWorldChoosingPanel();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static UWidgetWorldCreatingPanel* GetWidgetWorldCreatingPanel();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static UWidgetInventoryBar* GetWidgetInventoryBar();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static UWidgetInventoryPanel* GetWidgetInventoryPanel();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static UWidgetInventoryBox* GetWidgetInventoryBox();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static TSubclassOf<UWidgetCharacterHP> LoadWidgetCharacterHPClass();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static TSubclassOf<UWidgetVitalityHP> LoadWidgetVitalityHPClass();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static TSubclassOf<UWidgetWorldText> LoadWidgetWorldTextClass();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static TArray<FItemData> LoadItemDatas();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static FItemData LoadItemData(const FName& InItemID);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static TArray<FVoxelData> LoadVoxelDatas();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static FVoxelData LoadVoxelData(const FName& InVoxelID);
		
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static TArray<FEquipData> LoadEquipDatas();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static FEquipData LoadEquipData(const FName& InEquipID);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static TArray<FEquipWeaponData> LoadWeaponDatas();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static FEquipWeaponData LoadWeaponData(const FName& InWeaponID);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static TArray<FEquipShieldData> LoadShieldDatas();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static FEquipShieldData LoadShieldData(const FName& InShieldID);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static TArray<FEquipArmorData> LoadArmorDatas();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static FEquipArmorData LoadArmorData(const FName& InArmorID);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static TArray<FPropData> LoadPropDatas();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static FPropData LoadPropData(const FName& InPropID);
		
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static TArray<FVitalityRaceData> LoadVitalityRaceDatas();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static FVitalityRaceData LoadVitalityRaceData(const FName& InRaceID);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static FVitalityRaceData RandomVitalityRaceData();
		
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static TArray<FCharacterRaceData> LoadCharacterRaceDatas();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static FCharacterRaceData LoadCharacterRaceData(const FName& InRaceID);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DWHelper")
	static FCharacterRaceData RandomCharacterRaceData();
};
