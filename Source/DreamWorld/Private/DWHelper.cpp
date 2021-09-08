// Fill out your copyright notice in the Description page of Project Settings.


#include "DWHelper.h"
#include "World/WorldManager.h"
#include "Character/Player/DWPlayerCharacter.h"
#include "Character/Player/DWPlayerCharacterController.h"
#include "DWGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "DWGameInstance.h"
#include "DataSave/WorldDataSave.h"
#include "DWGameState.h"
#include "Engine/DataTable.h"
#include "Widget/Worlds/WidgetWorldText.h"
#include "Widget/Worlds/WidgetVitalityHP.h"
#include "Widget/Worlds/WidgetCharacterHP.h"
#include "DataSave/GameDataSave.h"
#include "DataSave/PlayerDataSave.h"
#include "Widget/WidgetPrimaryPanel.h"

ADWGameState* UDWHelper::CurrentGameState = nullptr;

ADWGameMode* UDWHelper::CurrentGameMode = nullptr;

UDWGameInstance* UDWHelper::CurrentGameInstance = nullptr;

ADWPlayerCharacterController* UDWHelper::CurrentPlayerController = nullptr;

ADWGameMode* UDWHelper::GetGameMode(const UObject* InWorldContext)
{
	if ((!CurrentGameMode || !CurrentGameMode->IsValidLowLevel()) && (InWorldContext && InWorldContext->IsValidLowLevel()))
	{
		CurrentGameMode = Cast<ADWGameMode>(UGameplayStatics::GetGameMode(InWorldContext));
	}
	return CurrentGameMode;
}

ADWGameState* UDWHelper::GetGameState(const UObject* InWorldContext)
{
	if ((!CurrentGameState || !CurrentGameState->IsValidLowLevel()) && (InWorldContext && InWorldContext->IsValidLowLevel()))
	{
		CurrentGameState = Cast<ADWGameState>(UGameplayStatics::GetGameState(InWorldContext));
	}
	return CurrentGameState;
}

UDWGameInstance* UDWHelper::GetGameInstance(const UObject* InWorldContext)
{
	if ((!CurrentGameInstance || !CurrentGameInstance->IsValidLowLevel()) && (InWorldContext && InWorldContext->IsValidLowLevel()))
	{
		CurrentGameInstance = Cast<UDWGameInstance>(UGameplayStatics::GetGameInstance(InWorldContext));
	}
	return CurrentGameInstance;
}

ADWPlayerCharacter* UDWHelper::GetPlayerCharacter(const UObject* InWorldContext)
{
	if (ADWPlayerCharacterController* PlayerController = GetPlayerController(InWorldContext))
	{
		return PlayerController->GetPlayerCharacter();
	}
	return nullptr;
}

ADWPlayerCharacterController* UDWHelper::GetPlayerController(const UObject* InWorldContext)
{
	if ((!CurrentPlayerController || !CurrentPlayerController->IsValidLowLevel()) && (InWorldContext && InWorldContext->IsValidLowLevel()))
	{
		CurrentPlayerController = Cast<ADWPlayerCharacterController>(UGameplayStatics::GetPlayerController(InWorldContext, 0));
	}
	return CurrentPlayerController;
}

TSubclassOf<UWidgetCharacterHP> UDWHelper::WidgetCharacterHPClass = nullptr;

TSubclassOf<UWidgetVitalityHP> UDWHelper::WidgetVitalityHPClass = nullptr;

TSubclassOf<UWidgetWorldText> UDWHelper::WidgetWorldTextClass = nullptr;

TSubclassOf<UWidgetCharacterHP> UDWHelper::LoadWidgetCharacterHPClass()
{
	if (!WidgetCharacterHPClass)
	{
		WidgetCharacterHPClass = LoadClass<UWidgetCharacterHP>(nullptr, TEXT("WidgetBlueprint'/Game/Blueprints/Widget/World/WB_CharacterHP.WB_CharacterHP_C'"));
	}
	return WidgetCharacterHPClass;
}

TSubclassOf<UWidgetVitalityHP> UDWHelper::LoadWidgetVitalityHPClass()
{
	if (!WidgetVitalityHPClass)
	{
		WidgetVitalityHPClass = LoadClass<UWidgetVitalityHP>(nullptr, TEXT("WidgetBlueprint'/Game/Blueprints/Widget/World/WB_VitalityHP.WB_VitalityHP_C'"));
	}
	return WidgetVitalityHPClass;
}

TSubclassOf<UWidgetWorldText> UDWHelper::LoadWidgetWorldTextClass()
{
	if (!WidgetWorldTextClass)
	{
		WidgetWorldTextClass = LoadClass<UWidgetWorldText>(nullptr, TEXT("WidgetBlueprint'/Game/Blueprints/Widget/World/WB_WorldText.WB_WorldText_C'"));
	}
	return WidgetWorldTextClass;
}

UDataTable* UDWHelper::VoxelsDataTable = nullptr;

UDataTable* UDWHelper::PropsDataTable = nullptr;

UDataTable* UDWHelper::SkillsDataTable = nullptr;

UDataTable* UDWHelper::WeaponsDataTable = nullptr;

UDataTable* UDWHelper::ShieldsDataTable = nullptr;

UDataTable* UDWHelper::ArmorsDataTable = nullptr;

UDataTable* UDWHelper::VitalityRacesDataTable = nullptr;

UDataTable* UDWHelper::CharacterRacesDataTable = nullptr;

TMap<FName, FItemData> UDWHelper::ItemDatas = TMap<FName, FItemData>();

TMap<FName, FVoxelData> UDWHelper::VoxelDatas = TMap<FName, FVoxelData>();

TMap<FName, FPropData> UDWHelper::PropDatas = TMap<FName, FPropData>();

TMap<FName, FSkillData> UDWHelper::SkillDatas = TMap<FName, FSkillData>();

TMap<FName, FEquipData> UDWHelper::EquipDatas = TMap<FName, FEquipData>();

TMap<FName, FEquipWeaponData> UDWHelper::WeaponDatas = TMap<FName, FEquipWeaponData>();

TMap<FName, FEquipShieldData> UDWHelper::ShieldDatas = TMap<FName, FEquipShieldData>();

TMap<FName, FEquipArmorData> UDWHelper::ArmorDatas = TMap<FName, FEquipArmorData>();

TMap<FName, FVitalityRaceData> UDWHelper::VitalityRaceDatas = TMap<FName, FVitalityRaceData>();

TMap<FName, FCharacterRaceData> UDWHelper::CharacterRaceDatas = TMap<FName, FCharacterRaceData>();

TArray<FItemData> UDWHelper::LoadItemDatas()
{
	if (ItemDatas.Num() == 0)
	{
		LoadVoxelDatas();
		LoadPropDatas();
		LoadSkillDatas();
		LoadEquipDatas();
	}
	TArray<FItemData> TmpArr;
	ItemDatas.GenerateValueArray(TmpArr);
	return TmpArr;
}

FItemData UDWHelper::LoadItemData(const FName& InItemID)
{
	if (ItemDatas.Num() == 0) LoadItemDatas();

	if (ItemDatas.Contains(InItemID))
	{
		return ItemDatas[InItemID];
	}
	return FItemData();
}

TArray<FVoxelData> UDWHelper::LoadVoxelDatas()
{
	if (VoxelsDataTable == nullptr)
	{
		VoxelsDataTable = LoadObject<UDataTable>(nullptr, TEXT("DataTable'/Game/DataTables/Voxel/DT_Voxels.DT_Voxels'"));
		if (VoxelsDataTable != nullptr)
		{
			FString ContextString;
			VoxelsDataTable->ForeachRow<FVoxelData>(ContextString, [](FName Key, FVoxelData Value){
				Value.ID = Key;
				Value.Type = EItemType::Voxel;
				VoxelDatas.Add(Key, Value);
				ItemDatas.Add(Key, Value);
			});
		}
	}
	TArray<FVoxelData> TmpArr;
	VoxelDatas.GenerateValueArray(TmpArr);
	return TmpArr;
}

FVoxelData UDWHelper::LoadVoxelData(const FName& InVoxelID)
{
	if (VoxelDatas.Num() == 0) LoadVoxelDatas();

	if (VoxelDatas.Contains(InVoxelID))
	{
		return VoxelDatas[InVoxelID];
	}
	return FVoxelData();
}

TArray<FPropData> UDWHelper::LoadPropDatas()
{
	if (PropsDataTable == nullptr)
	{
		PropsDataTable = LoadObject<UDataTable>(nullptr, TEXT("DataTable'/Game/DataTables/Prop/DT_Props.DT_Props'"));
		if (PropsDataTable != nullptr)
		{
			FString ContextString;
			PropsDataTable->ForeachRow<FPropData>(ContextString, [](FName Key, FPropData Value) {
				Value.ID = Key;
				Value.Type = EItemType::Prop;
				PropDatas.Add(Key, Value);
				ItemDatas.Add(Key, Value);
			});
		}
	}
	TArray<FPropData> TmpArr;
	PropDatas.GenerateValueArray(TmpArr);
	return TmpArr;
}

FPropData UDWHelper::LoadPropData(const FName& InPropID)
{
	if (PropDatas.Num() == 0) LoadPropDatas();

	if (PropDatas.Contains(InPropID))
	{
		return PropDatas[InPropID];
	}
	return FPropData();
}

TArray<FSkillData> UDWHelper::LoadSkillDatas()
{
	if (SkillsDataTable == nullptr)
	{
		SkillsDataTable = LoadObject<UDataTable>(nullptr, TEXT("DataTable'/Game/DataTables/Skill/DT_Skills.DT_Skills'"));
		if (SkillsDataTable != nullptr)
		{
			FString ContextString;
			SkillsDataTable->ForeachRow<FSkillData>(ContextString, [](FName Key, FSkillData Value) {
				Value.ID = Key;
				Value.Type = EItemType::Skill;
				SkillDatas.Add(Key, Value);
				ItemDatas.Add(Key, Value);
			});
		}
	}
	TArray<FSkillData> TmpArr;
	SkillDatas.GenerateValueArray(TmpArr);
	return TmpArr;
}

FSkillData UDWHelper::LoadSkillData(const FName& InSkillID)
{
	if (SkillDatas.Num() == 0) LoadSkillDatas();

	if (SkillDatas.Contains(InSkillID))
	{
		return SkillDatas[InSkillID];
	}
	return FSkillData();
}

TArray<FEquipData> UDWHelper::LoadEquipDatas()
{
	if (EquipDatas.Num() == 0)
	{
		LoadWeaponDatas();
		LoadShieldDatas();
		LoadArmorDatas();
	}
	TArray<FEquipData> TmpArr;
	EquipDatas.GenerateValueArray(TmpArr);
	return TmpArr;
}

FEquipData UDWHelper::LoadEquipData(const FName& InEquipID)
{
	if (EquipDatas.Num() == 0) LoadEquipDatas();

	if (EquipDatas.Contains(InEquipID))
	{
		return EquipDatas[InEquipID];
	}
	return FEquipData();
}

TArray<FEquipWeaponData> UDWHelper::LoadWeaponDatas()
{
	if (WeaponsDataTable == nullptr)
	{
		WeaponsDataTable = LoadObject<UDataTable>(nullptr, TEXT("DataTable'/Game/DataTables/Equip/Weapon/DT_Weapons.DT_Weapons'"));
		if (WeaponsDataTable != nullptr)
		{
			FString ContextString;
			WeaponsDataTable->ForeachRow<FEquipWeaponData>(ContextString, [](FName Key, FEquipWeaponData Value) {
				Value.ID = Key;
				Value.Type = EItemType::Equip;
				WeaponDatas.Add(Key, Value);
				EquipDatas.Add(Key, Value);
				ItemDatas.Add(Key, Value);
			});
		}
	}
	TArray<FEquipWeaponData> TmpArr;
	WeaponDatas.GenerateValueArray(TmpArr);
	return TmpArr;
}

FEquipWeaponData UDWHelper::LoadWeaponData(const FName& InWeaponID)
{
	if (WeaponDatas.Num() == 0) LoadWeaponDatas();

	if (WeaponDatas.Contains(InWeaponID))
	{
		return WeaponDatas[InWeaponID];
	}
	return FEquipWeaponData();
}

TArray<FEquipShieldData> UDWHelper::LoadShieldDatas()
{
	if (ShieldsDataTable == nullptr)
	{
		ShieldsDataTable = LoadObject<UDataTable>(nullptr, TEXT("DataTable'/Game/DataTables/Equip/Shield/DT_Shields.DT_Shields'"));
		if (ShieldsDataTable != nullptr)
		{
			FString ContextString;
			ShieldsDataTable->ForeachRow<FEquipShieldData>(ContextString, [](FName Key, FEquipShieldData Value) {
				Value.ID = Key;
				Value.Type = EItemType::Equip;
				ShieldDatas.Add(Key, Value);
				EquipDatas.Add(Key, Value);
				ItemDatas.Add(Key, Value);
			});
		}
	}
	TArray<FEquipShieldData> TmpArr;
	ShieldDatas.GenerateValueArray(TmpArr);
	return TmpArr;
}

FEquipShieldData UDWHelper::LoadShieldData(const FName& InShieldID)
{
	if (ShieldDatas.Num() == 0) LoadShieldDatas();

	if (ShieldDatas.Contains(InShieldID))
	{
		return ShieldDatas[InShieldID];
	}
	return FEquipShieldData();
}

TArray<FEquipArmorData> UDWHelper::LoadArmorDatas()
{
	if (ArmorsDataTable == nullptr)
	{
		ArmorsDataTable = LoadObject<UDataTable>(nullptr, TEXT("DataTable'/Game/DataTables/Equip/Armor/DT_Armors.DT_Armors'"));
		if (ArmorsDataTable != nullptr)
		{
			FString ContextString;
			ArmorsDataTable->ForeachRow<FEquipArmorData>(ContextString, [](FName Key, FEquipArmorData Value) {
				Value.ID = Key;
				Value.Type = EItemType::Equip;
				ArmorDatas.Add(Key, Value);
				EquipDatas.Add(Key, Value);
				ItemDatas.Add(Key, Value);
			});
		}
	}
	TArray<FEquipArmorData> TmpArr;
	ArmorDatas.GenerateValueArray(TmpArr);
	return TmpArr;
}

FEquipArmorData UDWHelper::LoadArmorData(const FName& InArmorID)
{
	if (ArmorDatas.Num() == 0) LoadArmorDatas();

	if (ArmorDatas.Contains(InArmorID))
	{
		return ArmorDatas[InArmorID];
	}
	return FEquipArmorData();
}

TArray<FVitalityRaceData> UDWHelper::LoadVitalityRaceDatas()
{
	if (VitalityRacesDataTable == nullptr)
	{
		VitalityRacesDataTable = LoadObject<UDataTable>(nullptr, TEXT("DataTable'/Game/DataTables/Race/DT_VitalityRaces.DT_VitalityRaces'"));
		if (VitalityRacesDataTable != nullptr)
		{
			FString ContextString;
			VitalityRacesDataTable->ForeachRow<FVitalityRaceData>(ContextString, [](FName Key, FVitalityRaceData Value) {
				Value.ID = Key;
				VitalityRaceDatas.Add(Key, Value);
			});
		}
	}
	TArray<FVitalityRaceData> TmpArr;
	VitalityRaceDatas.GenerateValueArray(TmpArr);
	return TmpArr;
}

FVitalityRaceData UDWHelper::LoadVitalityRaceData(const FName& InVitalityRaceID)
{
	if (VitalityRaceDatas.Num() == 0) LoadVoxelDatas();

	if (VitalityRaceDatas.Contains(InVitalityRaceID))
	{
		return VitalityRaceDatas[InVitalityRaceID];
	}
	return FVitalityRaceData();
}

FVitalityRaceData UDWHelper::RandomVitalityRaceData()
{
	auto raceDatas = UDWHelper::LoadVitalityRaceDatas();
	float tmpNum1 = 0;
	float tmpNum2 = 0;
	for (int32 i = 0; i < raceDatas.Num(); i++)
	{
		tmpNum1 += raceDatas[i].Proportion;
	}
	tmpNum1 = FMath::FRandRange(0, tmpNum1);
	for (int32 i = 0; i < raceDatas.Num(); i++)
	{
		tmpNum2 += raceDatas[i].Proportion;
		if (tmpNum1 <= tmpNum2)
		{
			return raceDatas[i];
		}
	}
	return FVitalityRaceData();
}

TArray<FCharacterRaceData> UDWHelper::LoadCharacterRaceDatas()
{
	if (CharacterRacesDataTable == nullptr)
	{
		CharacterRacesDataTable = LoadObject<UDataTable>(nullptr, TEXT("DataTable'/Game/DataTables/Race/DT_CharacterRaces.DT_CharacterRaces'"));
		if (CharacterRacesDataTable != nullptr)
		{
			FString ContextString;
			CharacterRacesDataTable->ForeachRow<FCharacterRaceData>(ContextString, [](FName Key, FCharacterRaceData Value) {
				Value.ID = Key;
				CharacterRaceDatas.Add(Key, Value);
			});
		}
	}
	TArray<FCharacterRaceData> TmpArr;
	CharacterRaceDatas.GenerateValueArray(TmpArr);
	return TmpArr;
}

FCharacterRaceData UDWHelper::LoadCharacterRaceData(const FName& InCharacterRaceID)
{
	if (CharacterRaceDatas.Num() == 0) LoadVoxelDatas();

	if (CharacterRaceDatas.Contains(InCharacterRaceID))
	{
		return CharacterRaceDatas[InCharacterRaceID];
	}
	return FCharacterRaceData();
}

FCharacterRaceData UDWHelper::RandomCharacterRaceData()
{
	auto RaceDatas = UDWHelper::LoadCharacterRaceDatas();
	float TmpNum1 = 0;
	float TmpNum2 = 0;
	
	for (int32 i = 0; i < RaceDatas.Num(); i++)
	{
		TmpNum1 += RaceDatas[i].Proportion;
	}
	TmpNum1 = FMath::FRandRange(0, TmpNum1);
	for (int32 i = 0; i < RaceDatas.Num(); i++)
	{
		TmpNum2 += RaceDatas[i].Proportion;
		if (TmpNum1 <= TmpNum2)
		{
			return RaceDatas[i];
		}
	}
	return FCharacterRaceData();
}

UWidgetPanelBase* UDWHelper::K2_GetWidgetPanelByClass(const UObject* InWorldContext, TSubclassOf<UWidgetPanelBase> InWidgetPanelClass)
{
	if(ADWGameMode* GameMode = GetGameMode(InWorldContext))
	{
		return GameMode->K2_GetWidgetPanelByClass(InWidgetPanelClass);
	}
	return nullptr;
}

void UDWHelper::Debug(const FString& Message, EDebugType DebugType, float Duration)
{
	switch (DebugType)
	{
		case EDebugType::Screen:
		{
			GEngine->AddOnScreenDebugMessage(-1, Duration, FColor::Cyan, Message, false);
			break;
		}
		case EDebugType::Console:
		{
			UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
			break;
		}
		case EDebugType::Both:
		{
			Debug(Message, EDebugType::Screen, Duration);
			Debug(Message, EDebugType::Console);
			break;
		}
	}
}

FString UDWHelper::EnumValueToString(const FString& InEnumName, const int32& InEnumValue)
{
	if(UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, *InEnumName, true))
	{
		return EnumPtr->GetAuthoredNameStringByValue(InEnumValue);
	}
	return TEXT("");
}

EDirection UDWHelper::InvertDirection(EDirection InDirection)
{
	if ((int)InDirection % 2 == 0)
		return (EDirection)((int)InDirection + 1);
	else
		return (EDirection)((int)InDirection - 1);
}

FVector UDWHelper::DirectionToVector(EDirection InDirection, FRotator InRotation /*= FRotator::ZeroRotator*/)
{
	switch (InDirection)
	{
		case EDirection::Up:
			return InRotation.RotateVector(FVector::UpVector);
		case EDirection::Down:
			return InRotation.RotateVector(FVector::DownVector);
		case EDirection::Forward:
			return InRotation.RotateVector(FVector::ForwardVector);
		case EDirection::Back:
			return InRotation.RotateVector(FVector::BackwardVector);
		case EDirection::Left:
			return InRotation.RotateVector(FVector::LeftVector);
		case EDirection::Right:
			return InRotation.RotateVector(FVector::RightVector);
	}
	return FVector::ZeroVector;
}

FIndex UDWHelper::DirectionToIndex(EDirection InDirection, FRotator InRotation /*= FRotator::ZeroRotator*/)
{
	return FIndex(DirectionToVector(InDirection, InRotation));
}

FIndex UDWHelper::GetAdjacentIndex(FIndex InIndex, EDirection InDirection, FRotator InRotation /*= FRotator::ZeroRotator*/)
{
	return InIndex + DirectionToIndex(InDirection, InRotation);
}

ETraceTypeQuery UDWHelper::GetGameTrace(EGameTraceType InGameTraceType)
{
	return UEngineTypes::ConvertToTraceType((ECollisionChannel)InGameTraceType);
}
