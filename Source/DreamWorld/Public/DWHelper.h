// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DWHelper.generated.h"

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

	//////////////////////////////////////////////////////////////////////////
	// References
private:
	static class ADWGameMode* CurrentGameMode;

	static class ADWGameState* CurrentGameState;

	static class UDWGameInstance* CurrentGameInstance;

	static class ADWPlayerController* CurrentPlayerController;

public:
	UFUNCTION(BlueprintPure, meta = (WorldContext = "InWorldContext"), Category = "DWHelper")
	static class ADWGameMode* GetGameMode(const UObject* InWorldContext);

	UFUNCTION(BlueprintPure, meta = (WorldContext = "InWorldContext"), Category = "DWHelper")
	static class ADWGameState* GetGameState(const UObject* InWorldContext);
	
	UFUNCTION(BlueprintPure, meta = (WorldContext = "InWorldContext"), Category = "DWHelper")
	static class UDWGameInstance* GetGameInstance(const UObject* InWorldContext);
		
	UFUNCTION(BlueprintPure, meta = (WorldContext = "InWorldContext"), Category = "DWHelper")
	static class ADWPlayerCharacter* GetPlayerCharacter(const UObject* InWorldContext);
	
	UFUNCTION(BlueprintPure, meta = (WorldContext = "InWorldContext"), Category = "DWHelper")
	static class ADWPlayerController* GetPlayerController(const UObject* InWorldContext);

	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static class ADWMainModule* GetMainModule();

	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static class AWorldManager* GetWorldManager();

	//////////////////////////////////////////////////////////////////////////
	// Datas
private:
	static UDataTable* VoxelsDataTable;
	
	static UDataTable* PropsDataTable;

	static UDataTable* SkillsDataTable;

	static UDataTable* WeaponsDataTable;

	static UDataTable* ShieldsDataTable;

	static UDataTable* ArmorsDataTable;

	static UDataTable* VitalitiesDataTable;

	static UDataTable* CharactersDataTable;

	static UDataTable* VitalityRacesDataTable;

	static UDataTable* CharacterRacesDataTable;

	static TMap<FName, FItemData> ItemDatas;

	static TMap<FName, FVoxelData> VoxelDatas;

	static TMap<FName, FPropData> PropDatas;
		
	static TMap<FName, FSkillData> SkillDatas;

	static TMap<FName, FEquipData> EquipDatas;

	static TMap<FName, FEquipWeaponData> WeaponDatas;

	static TMap<FName, FEquipShieldData> ShieldDatas;

	static TMap<FName, FEquipArmorData> ArmorDatas;

	static TMap<FName, FVitalityData> VitalityDatas;

	static TMap<FName, FCharacterData> CharacterDatas;

	static TMap<FName, FVitalityRaceData> VitalityRaceDatas;

	static TMap<FName, FCharacterRaceData> CharacterRaceDatas;

public:
	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static TArray<FItemData> LoadItemDatas();

	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static FItemData LoadItemData(const FName& InItemID);
	
	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static TArray<FVoxelData> LoadVoxelDatas();

	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static FVoxelData LoadVoxelData(const FName& InVoxelID);

	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static TArray<FPropData> LoadPropDatas();

	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static FPropData LoadPropData(const FName& InPropID);
	
	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static TArray<FSkillData> LoadSkillDatas();

	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static FSkillData LoadSkillData(const FName& InSkillID);
		
	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static TArray<FEquipData> LoadEquipDatas();

	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static FEquipData LoadEquipData(const FName& InEquipID);
	
	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static TArray<FEquipWeaponData> LoadWeaponDatas();

	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static FEquipWeaponData LoadWeaponData(const FName& InWeaponID);
	
	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static TArray<FEquipShieldData> LoadShieldDatas();

	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static FEquipShieldData LoadShieldData(const FName& InShieldID);
	
	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static TArray<FEquipArmorData> LoadArmorDatas();

	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static FEquipArmorData LoadArmorData(const FName& InArmorID);
						
	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static TArray<FVitalityData> LoadVitalityDatas();

	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static FVitalityData LoadVitalityData(const FName& InVitalityID);
		
	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static TArray<FCharacterData> LoadCharacterDatas();

	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static FCharacterData LoadCharacterData(const FName& InCharacterID);

	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static TArray<FVitalityRaceData> LoadVitalityRaceDatas();

	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static FVitalityRaceData LoadVitalityRaceData(const FName& InRaceID);

	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static FVitalityRaceData RandomVitalityRaceData();
		
	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static TArray<FCharacterRaceData> LoadCharacterRaceDatas();

	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static FCharacterRaceData LoadCharacterRaceData(const FName& InRaceID);

	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static FCharacterRaceData RandomCharacterRaceData();

	//////////////////////////////////////////////////////////////////////////
	// Classes
private:
	static TSubclassOf<class UWidgetMainMenu> WidgetMainMenuClass;

	static TSubclassOf<class UWidgetSettingPanel> WidgetSettingPanelClass;
	
	static TSubclassOf<class UWidgetLoadingPanel> WidgetLoadingPanelClass;

	static TSubclassOf<class UWidgetPausingMenu> WidgetPausingMenuClass;

	static TSubclassOf<class UWidgetPrimaryPanel> WidgetPrimaryPanelClass;

	static TSubclassOf<class UWidgetRoleChoosingPanel> WidgetRoleChoosingPanelClass;

	static TSubclassOf<class UWidgetRoleCreatingPanel> WidgetRoleCreatingPanelClass;

	static TSubclassOf<class UWidgetWorldChoosingPanel> WidgetWorldChoosingPanelClass;

	static TSubclassOf<class UWidgetWorldCreatingPanel> WidgetWorldCreatingPanelClass;

	static TSubclassOf<class UWidgetInventoryBar> WidgetInventoryBarClass;

	static TSubclassOf<class UWidgetInventoryPanel> WidgetInventoryPanelClass;

	static TSubclassOf<class UWidgetCharacterHP> WidgetCharacterHPClass;

	static TSubclassOf<class UWidgetVitalityHP> WidgetVitalityHPClass;

	static TSubclassOf<class UWidgetWorldText> WidgetWorldTextClass;

public:
	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static TSubclassOf<class UWidgetMainMenu> LoadWidgetMainMenuClass();

	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static TSubclassOf<class UWidgetSettingPanel> LoadWidgetSettingPanelClass();

	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static TSubclassOf<class UWidgetLoadingPanel> LoadWidgetLoadingPanelClass();

	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static TSubclassOf<class UWidgetPausingMenu> LoadWidgetPausingMenuClass();

	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static TSubclassOf<class UWidgetPrimaryPanel> LoadWidgetPrimaryPanelClass();

	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static TSubclassOf<class UWidgetRoleChoosingPanel> LoadWidgetRoleChoosingPanelClass();

	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static TSubclassOf<class UWidgetRoleCreatingPanel> LoadWidgetRoleCreatingPanelClass();

	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static TSubclassOf<class UWidgetWorldChoosingPanel> LoadWidgetWorldChoosingPanelClass();

	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static TSubclassOf<class UWidgetWorldCreatingPanel> LoadWidgetWorldCreatingPanelClass();

	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static TSubclassOf<class UWidgetInventoryBar> LoadWidgetInventoryBarClass();

	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static TSubclassOf<class UWidgetInventoryPanel> LoadWidgetInventoryPanelClass();

	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static TSubclassOf<class UWidgetCharacterHP> LoadWidgetCharacterHPClass();
	
	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static TSubclassOf<class UWidgetVitalityHP> LoadWidgetVitalityHPClass();

	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static TSubclassOf<class UWidgetWorldText> LoadWidgetWorldTextClass();

	//////////////////////////////////////////////////////////////////////////
	// Index
public:
	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static EDirection InvertDirection(EDirection InDirection);

	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static FVector DirectionToVector(EDirection InDirection, FRotator InRotation = FRotator::ZeroRotator);

	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static FIndex DirectionToIndex(EDirection InDirection, FRotator InRotation = FRotator::ZeroRotator);

	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static FIndex GetAdjacentIndex(FIndex InIndex, EDirection InDirection, FRotator InRotation = FRotator::ZeroRotator);

	//////////////////////////////////////////////////////////////////////////
	// Trace
public:
	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static ETraceTypeQuery GetGameTrace(EGameTraceType InGameTraceType);
};
