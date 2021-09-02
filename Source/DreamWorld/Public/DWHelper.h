// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DWGameMode.h"
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
	static ADWGameMode* CurrentGameMode;

	static ADWGameState* CurrentGameState;

	static UDWGameInstance* CurrentGameInstance;

	static ADWPlayerCharacterController* CurrentPlayerController;

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
	static class ADWPlayerCharacterController* GetPlayerController(const UObject* InWorldContext);
	
	//////////////////////////////////////////////////////////////////////////
	// DataSaves
public:
	UFUNCTION(BlueprintPure, meta = (WorldContext = "InWorldContext"), Category = "DWHelper")
	static class UGameDataSave* GetGameDataSave(const UObject* InWorldContext);

	UFUNCTION(BlueprintPure, meta = (WorldContext = "InWorldContext"), Category = "DWHelper")
	static class UWorldDataSave* GetWorldDataSave(const UObject* InWorldContext);

	UFUNCTION(BlueprintPure, meta = (WorldContext = "InWorldContext"), Category = "DWHelper")
	static class UPlayerDataSave* GetPlayerDataSave(const UObject* InWorldContext);

	//////////////////////////////////////////////////////////////////////////
	// Datas
private:
	static UDataTable* VoxelsDataTable;
	
	static UDataTable* PropsDataTable;

	static UDataTable* SkillsDataTable;

	static UDataTable* WeaponsDataTable;

	static UDataTable* ShieldsDataTable;

	static UDataTable* ArmorsDataTable;

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
	static TSubclassOf<UWidgetVitalityHP> WidgetVitalityHPClass;

	static TSubclassOf<UWidgetCharacterHP> WidgetCharacterHPClass;

	static TSubclassOf<UWidgetWorldText> WidgetWorldTextClass;

public:
	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static TSubclassOf<UWidgetCharacterHP> LoadWidgetCharacterHPClass();
	
	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static TSubclassOf<UWidgetVitalityHP> LoadWidgetVitalityHPClass();

	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static TSubclassOf<UWidgetWorldText> LoadWidgetWorldTextClass();

	//////////////////////////////////////////////////////////////////////////
	// WidgetPanels
public:
	template<class T>
	static T* GetWidgetPanelByClass(const UObject* InWorldContext)
	{
		if(ADWGameMode* GameMode = GetGameMode(InWorldContext))
		{
			return GameMode->GetWidgetPanelByClass<T>();
		}
		return nullptr;
	}

	UFUNCTION(BlueprintPure, meta = (WorldContext = "InWorldContext", DisplayName = "GetWidgetPanelByClass", DeterminesOutputType = "InWidgetPanelClass"), Category = "DWHelper")
	static class UWidgetPanelBase* K2_GetWidgetPanelByClass(const UObject* InWorldContext, TSubclassOf<UWidgetPanelBase> InWidgetPanelClass);

	//////////////////////////////////////////////////////////////////////////
	// Debug
public:
	UFUNCTION(BlueprintCallable, Category = "DWHelper")
	static void Debug(const FString& Message, EDebugType DebugType = EDebugType::Screen, float Duration = 1.5f);

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
	// Enum
public:
	UFUNCTION(BlueprintPure, Category = "DWHelper")
	static FString EnumValueToString(const FString& InEnumName, const int32& InEnumValue);
};
