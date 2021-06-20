// Fill out your copyright notice in the Description page of Project Settings.


#include "DWHelper.h"
#include "World/WorldManager.h"
#include "World/Chunk.h"
#include "Voxel/Voxel.h"
#include "Character/Player/DWPlayerCharacter.h"
#include "Character/Player/DWPlayerCharacterController.h"
#include "DWGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "ConstructorHelpers.h"
#include "DWGameInstance.h"
#include "DataSaves/WorldDataSave.h"
#include "DWGameState.h"
#include "Engine/DataTable.h"
#include "Widget/Worlds/WidgetWorldText.h"
#include "Widget/Worlds/WidgetVitalityHP.h"
#include "Widget/Worlds/WidgetCharacterHP.h"
#include "DataSaves/GameDataSave.h"
#include "DataSaves/WorldDataSave.h"
#include "DataSaves/PlayerDataSave.h"
#include "Widget/WidgetPrimaryPanel.h"
#include "Widget/WidgetMainMenu.h"
#include "Widget/WidgetPausingMenu.h"
#include "Widget/WidgetLoadingPanel.h"
#include "Widget/WidgetSettingPanel.h"
#include "Widget/WidgetRoleChoosingPanel.h"
#include "Widget/WidgetRoleCreatingPanel.h"
#include "Widget/WidgetWorldChoosingPanel.h"
#include "Widget/WidgetWorldCreatingPanel.h"
#include "Widget/Inventory/WidgetInventoryBar.h"
#include "Widget/Inventory/WidgetInventoryPanel.h"
#include "Widget/Inventory/WidgetInventoryBox.h"

UWorld* UDWHelper::World = nullptr;

AWorldManager* UDWHelper::WorldManager = nullptr;

ADWGameState* UDWHelper::GameState = nullptr;

ADWGameMode* UDWHelper::GameMode = nullptr;

UDWGameInstance* UDWHelper::GameInstance = nullptr;

ADWPlayerCharacterController* UDWHelper::PlayerController = nullptr;

TSubclassOf<UWidgetCharacterHP> UDWHelper::WidgetCharacterHPClass = nullptr;

TSubclassOf<UWidgetVitalityHP> UDWHelper::WidgetVitalityHPClass = nullptr;

TSubclassOf<UWidgetWorldText> UDWHelper::WidgetWorldTextClass = nullptr;

TMap<FName, FItemData> UDWHelper::ItemDatas = TMap<FName, FItemData>();

TMap<FName, FVoxelData> UDWHelper::VoxelDatas = TMap<FName, FVoxelData>();

TMap<FName, FEquipData> UDWHelper::EquipDatas = TMap<FName, FEquipData>();

TMap<FName, FEquipWeaponData> UDWHelper::WeaponDatas = TMap<FName, FEquipWeaponData>();

TMap<FName, FEquipShieldData> UDWHelper::ShieldDatas = TMap<FName, FEquipShieldData>();

TMap<FName, FEquipArmorData> UDWHelper::ArmorDatas = TMap<FName, FEquipArmorData>();

TMap<FName, FPropData> UDWHelper::PropDatas = TMap<FName, FPropData>();

TMap<FName, FVitalityRaceData> UDWHelper::VitalityRaceDatas = TMap<FName, FVitalityRaceData>();

TMap<FName, FCharacterRaceData> UDWHelper::CharacterRaceDatas = TMap<FName, FCharacterRaceData>();

void UDWHelper::Debug(const FString& Message, EDebugType DebugType, float Duration)
{
	switch (DebugType)
	{
		case EDebugType::Screen:
		{
			GEngine->AddOnScreenDebugMessage(0, Duration, FColor::Cyan, Message, false);
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

void UDWHelper::Initialize(UWorld* InWorld)
{
	World = InWorld;
}

AWorldManager* UDWHelper::GetWorldManager()
{
	if ((!WorldManager || !WorldManager->IsValidLowLevel()) && (World && World->IsValidLowLevel()))
	{
		WorldManager = Cast<AWorldManager>(UGameplayStatics::GetActorOfClass(World, AWorldManager::StaticClass()));
	}
	return WorldManager;
}

ADWGameState* UDWHelper::GetGameState()
{
	if ((!GameState || !GameState->IsValidLowLevel()) && (World && World->IsValidLowLevel()))
	{
		GameState = Cast<ADWGameState>(UGameplayStatics::GetGameState(World));
	}
	return GameState;
}

ADWGameMode* UDWHelper::GetGameMode()
{
	if ((!GameMode || !GameMode->IsValidLowLevel()) && (World && World->IsValidLowLevel()))
	{
		GameMode = Cast<ADWGameMode>(UGameplayStatics::GetGameMode(World));
	}
	return GameMode;
}

UDWGameInstance* UDWHelper::GetGameInstance()
{
	if ((!GameInstance || !GameInstance->IsValidLowLevel()) && (World && World->IsValidLowLevel()))
	{
		GameInstance = Cast<UDWGameInstance>(UGameplayStatics::GetGameInstance(World));
	}
	return GameInstance;
}

ADWPlayerCharacter* UDWHelper::GetPlayerCharacter()
{
	if (GetPlayerController())
	{
		return GetPlayerController()->GetPossessedCharacter();
	}
	return nullptr;
}

ADWPlayerCharacterController* UDWHelper::GetPlayerController()
{
	if ((!PlayerController || !PlayerController->IsValidLowLevel()) && (World && World->IsValidLowLevel()))
	{
		PlayerController = Cast<ADWPlayerCharacterController>(UGameplayStatics::GetPlayerController(World, 0));
	}
	return PlayerController;
}

UGameDataSave* UDWHelper::GetGameDataSave()
{
	if (GetGameInstance())
	{
		return GetGameInstance()->GetGameDataSave();
	}
	return nullptr;
}

UWorldDataSave* UDWHelper::GetWorldDataSave()
{
	if (GetGameInstance())
	{
		return GetGameInstance()->GetWorldDataSave();
	}
	return nullptr;
}

UPlayerDataSave* UDWHelper::GetPlayerDataSave()
{
	if (GetGameInstance())
	{
		return GetGameInstance()->GetPlayerDataSave();
	}
	return nullptr;
}

UWidgetPrimaryPanel* UDWHelper::GetWidgetPrimaryPanel()
{
	if (GetGameMode())
	{
		return Cast<UWidgetPrimaryPanel>(GetGameMode()->GetWidgetPanel(FName("PrimaryPanel")));
	}
	return nullptr;
}

UWidgetMainMenu* UDWHelper::GetWidgetMainMenu()
{
	if (GetGameMode())
	{
		return Cast<UWidgetMainMenu>(GetGameMode()->GetWidgetPanel(FName("MainMenu")));
	}
	return nullptr;
}

UWidgetPausingMenu* UDWHelper::GetWidgetPauseMenu()
{
	if (GetGameMode())
	{
		return Cast<UWidgetPausingMenu>(GetGameMode()->GetWidgetPanel(FName("PausingMenu")));
	}
	return nullptr;
}

UWidgetLoadingPanel* UDWHelper::GetWidgetLoadingPanel()
{
	if (GetGameMode())
	{
		return Cast<UWidgetLoadingPanel>(GetGameMode()->GetWidgetPanel(FName("LoadingPanel")));
	}
	return nullptr;
}

UWidgetSettingPanel* UDWHelper::GetWidgetSettingPanel()
{
	if (GetGameMode())
	{
		return Cast<UWidgetSettingPanel>(GetGameMode()->GetWidgetPanel(FName("SettingPanel")));
	}
	return nullptr;
}

UWidgetRoleChoosingPanel* UDWHelper::GetWidgetRoleChoosingPanel()
{
	if (GetGameMode())
	{
		return Cast<UWidgetRoleChoosingPanel>(GetGameMode()->GetWidgetPanel(FName("RoleChoosingPanel")));
	}
	return nullptr;
}

UWidgetRoleCreatingPanel* UDWHelper::GetWidgetRoleCreatingPanel()
{
	if (GetGameMode())
	{
		return Cast<UWidgetRoleCreatingPanel>(GetGameMode()->GetWidgetPanel(FName("RoleCreatingPanel")));
	}
	return nullptr;
}

UWidgetWorldChoosingPanel* UDWHelper::GetWidgetWorldChoosingPanel()
{
	if (GetGameMode())
	{
		return Cast<UWidgetWorldChoosingPanel>(GetGameMode()->GetWidgetPanel(FName("WorldChoosingPanel")));
	}
	return nullptr;
}

UWidgetWorldCreatingPanel* UDWHelper::GetWidgetWorldCreatingPanel()
{
	if (GetGameMode())
	{
		return Cast<UWidgetWorldCreatingPanel>(GetGameMode()->GetWidgetPanel(FName("WorldCreatingPanel")));
	}
	return nullptr;
}

UWidgetInventoryBar* UDWHelper::GetWidgetInventoryBar()
{
	if (GetGameMode())
	{
		return Cast<UWidgetInventoryBar>(GetGameMode()->GetWidgetPanel(FName("InventoryBar")));
	}
	return nullptr;
}

UWidgetInventoryPanel* UDWHelper::GetWidgetInventoryPanel()
{
	if (GetGameMode())
	{
		return Cast<UWidgetInventoryPanel>(GetGameMode()->GetWidgetPanel(FName("InventoryPanel")));
	}
	return nullptr;
}

UWidgetInventoryBox* UDWHelper::GetWidgetInventoryBox()
{
	if (GetGameMode())
	{
		return Cast<UWidgetInventoryBox>(GetGameMode()->GetWidgetPanel(FName("InventoryBox")));
	}
	return nullptr;
}

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

TArray<FItemData> UDWHelper::LoadItemDatas()
{
	if (ItemDatas.Num() == 0)
	{
		LoadVoxelDatas();
		LoadEquipDatas();
		LoadPropDatas();
	}
	TArray<FItemData> tmpArr;
	ItemDatas.GenerateValueArray(tmpArr);
	return tmpArr;
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
	if (VoxelDatas.Num() == 0)
	{
		UDataTable* dataTable = LoadObject<UDataTable>(nullptr, TEXT("DataTable'/Game/DataTables/Voxel/DT_Voxels.DT_Voxels'"));
		if (dataTable != nullptr)
		{
			FString contextString;
			dataTable->ForeachRow<FVoxelData>(contextString, [](FName key, FVoxelData value){
				value.ID = key;
				value.Type = EItemType::Voxel;
				VoxelDatas.Add(key, value);
				ItemDatas.Add(key, value);
			});
		}
	}
	TArray<FVoxelData> tmpArr;
	VoxelDatas.GenerateValueArray(tmpArr);
	return tmpArr;
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

TArray<FEquipData> UDWHelper::LoadEquipDatas()
{
	if (EquipDatas.Num() == 0)
	{
		LoadWeaponDatas();
		LoadShieldDatas();
		LoadArmorDatas();
	}
	TArray<FEquipData> tmpArr;
	EquipDatas.GenerateValueArray(tmpArr);
	return tmpArr;
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
	if (WeaponDatas.Num() == 0)
	{
		UDataTable* dataTable = LoadObject<UDataTable>(nullptr, TEXT("DataTable'/Game/DataTables/Equip/Weapon/DT_Weapons.DT_Weapons'"));
		if (dataTable != nullptr)
		{
			FString contextString;
			dataTable->ForeachRow<FEquipWeaponData>(contextString, [](FName key, FEquipWeaponData value) {
				value.ID = key;
				value.Type = EItemType::Equip;
				WeaponDatas.Add(key, value);
				EquipDatas.Add(key, value);
				ItemDatas.Add(key, value);
			});
		}
	}
	TArray<FEquipWeaponData> tmpArr;
	WeaponDatas.GenerateValueArray(tmpArr);
	return tmpArr;
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
	if (ShieldDatas.Num() == 0)
	{
		UDataTable* dataTable = LoadObject<UDataTable>(nullptr, TEXT("DataTable'/Game/DataTables/Equip/Shield/DT_Shields.DT_Shields'"));
		if (dataTable != nullptr)
		{
			FString contextString;
			dataTable->ForeachRow<FEquipShieldData>(contextString, [](FName key, FEquipShieldData value) {
				value.ID = key;
				value.Type = EItemType::Equip;
				ShieldDatas.Add(key, value);
				EquipDatas.Add(key, value);
				ItemDatas.Add(key, value);
			});
		}
	}
	TArray<FEquipShieldData> tmpArr;
	ShieldDatas.GenerateValueArray(tmpArr);
	return tmpArr;
}

FEquipShieldData UDWHelper::LoadShieldData(const FName& InShieldID)
{
	if (ShieldDatas.Num() == 0) LoadShieldDatas();

	if (ShieldDatas.Contains(InShieldID))
	{
		return ShieldDatas[InShieldID];
	}
	return static FEquipShieldData();
}

TArray<FEquipArmorData> UDWHelper::LoadArmorDatas()
{
	if (ArmorDatas.Num() == 0)
	{
		UDataTable* dataTable = LoadObject<UDataTable>(nullptr, TEXT("DataTable'/Game/DataTables/Equip/Armor/DT_Armors.DT_Armors'"));
		if (dataTable != nullptr)
		{
			FString contextString;
			dataTable->ForeachRow<FEquipArmorData>(contextString, [](FName key, FEquipArmorData value) {
				value.ID = key;
				value.Type = EItemType::Equip;
				ArmorDatas.Add(key, value);
				EquipDatas.Add(key, value);
				ItemDatas.Add(key, value);
			});
		}
	}
	TArray<FEquipArmorData> tmpArr;
	ArmorDatas.GenerateValueArray(tmpArr);
	return tmpArr;
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

TArray<FPropData> UDWHelper::LoadPropDatas()
{
	if (PropDatas.Num() == 0)
	{
		UDataTable* dataTable = LoadObject<UDataTable>(nullptr, TEXT("DataTable'/Game/DataTables/Prop/DT_Props.DT_Props'"));
		if (dataTable != nullptr)
		{
			FString contextString;
			dataTable->ForeachRow<FPropData>(contextString, [](FName key, FPropData value) {
				value.ID = key;
				value.Type = EItemType::Prop;
				PropDatas.Add(key, value);
				ItemDatas.Add(key, value);
			});
		}
	}
	TArray<FPropData> tmpArr;
	PropDatas.GenerateValueArray(tmpArr);
	return tmpArr;
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

TArray<FVitalityRaceData> UDWHelper::LoadVitalityRaceDatas()
{
	if (VitalityRaceDatas.Num() == 0)
	{
		UDataTable* dataTable = LoadObject<UDataTable>(nullptr, TEXT("DataTable'/Game/DataTables/Race/DT_VitalityRaces.DT_VitalityRaces'"));
		if (dataTable != nullptr)
		{
			FString contextString;
			dataTable->ForeachRow<FVitalityRaceData>(contextString, [](FName key, FVitalityRaceData value) {
				value.ID = key;
				VitalityRaceDatas.Add(key, value);
			});
		}
	}
	TArray<FVitalityRaceData> tmpArr;
	VitalityRaceDatas.GenerateValueArray(tmpArr);
	return tmpArr;
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
	if (CharacterRaceDatas.Num() == 0)
	{
		UDataTable* dataTable = LoadObject<UDataTable>(nullptr, TEXT("DataTable'/Game/DataTables/Race/DT_CharacterRaces.DT_CharacterRaces'"));
		if (dataTable != nullptr)
		{
			FString contextString;
			dataTable->ForeachRow<FCharacterRaceData>(contextString, [](FName key, FCharacterRaceData value) {
				value.ID = key;
				CharacterRaceDatas.Add(key, value);
				});
		}
	}
	TArray<FCharacterRaceData> tmpArr;
	CharacterRaceDatas.GenerateValueArray(tmpArr);
	return tmpArr;
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
	auto raceDatas = UDWHelper::LoadCharacterRaceDatas();
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
	return FCharacterRaceData();
}
