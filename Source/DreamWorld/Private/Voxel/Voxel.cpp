// Fill out your copyright notice in the Description page of Project Settings.


#include "Voxel/Voxel.h"
#include "World/Chunk.h"
#include "DWGameMode.h"
#include "World/WorldManager.h"
#include "PickUp/PickUpVoxel.h"
#include "Widget/Inventory/WidgetInventoryBar.h"
#include "Widget/Inventory/Slot/WidgetInventorySlot.h"
#include "Character/Player/DWPlayerCharacter.h"
#include "Engine/World.h"
#include "DWGameInstance.h"
#include "ObjectPoolModuleBPLibrary.h"
#include "VoxelAuxiliary/VoxelAuxiliary.h"
#include "Inventory/CharacterInventory.h"
#include "Inventory/Slot/InventorySlot.h"
#include "SharedPointer.h"
#include "WidgetModuleBPLibrary.h"
#include "DataSave/WorldDataSave.h"
#include "Kismet/GameplayStatics.h"

UVoxel* UVoxel::EmptyVoxel = nullptr;

UVoxel* UVoxel::UnknownVoxel = nullptr;

UVoxel::UVoxel()
{
	ID = NAME_None;
	Index = FIndex::ZeroIndex;
	Rotation = FRotator::ZeroRotator;
	Scale = FVector::OneVector;
	Params = TMap<FName, FParameter>();
	Owner = nullptr;
	Auxiliary = nullptr;
}

UVoxel* UVoxel::SpawnVoxel(UObject* InWorldContext, EVoxelType InVoxelType)
{
	return SpawnVoxel(InWorldContext, *FString::Printf(TEXT("Voxel_%d"), (int32)InVoxelType));
}

UVoxel* UVoxel::SpawnVoxel(UObject* InWorldContext, const FName& InVoxelID)
{
	FVoxelData voxelData = UDWHelper::LoadVoxelData(InVoxelID);
	TSubclassOf<UVoxel> tmpClass = UVoxel::StaticClass();
	if (voxelData.IsValid()) tmpClass = voxelData.VoxelClass;
	auto voxel = UObjectPoolModuleBPLibrary::SpawnObject<UVoxel>(InWorldContext, tmpClass);
	voxel->ID = InVoxelID;
	return voxel;
}

UVoxel* UVoxel::LoadVoxel(AChunk* InOwner, const FVoxelItem& InVoxelItem)
{
	auto voxel = SpawnVoxel(InOwner, InVoxelItem.ID);
	voxel->LoadItem(InVoxelItem);
	voxel->SetOwner(InOwner);
	voxel->SetAuxiliary(InVoxelItem.Auxiliary);
	return voxel;
}

UVoxel* UVoxel::LoadVoxel(AChunk* InOwner, const FString& InVoxelData)
{
	FString str1, str2;
	InVoxelData.Split(TEXT(";"), &str1, &str2);
	auto voxel = SpawnVoxel(InOwner, *str1);
	voxel->LoadData(str2);
	voxel->SetOwner(InOwner);
	return voxel;
}

void UVoxel::DespawnVoxel(UObject* InWorldContext, UVoxel* InVoxel)
{
	if(UVoxel::IsValid(InVoxel, false))
	{
		UObjectPoolModuleBPLibrary::DespawnObject(InWorldContext, InVoxel);
	}
}

bool UVoxel::IsValid(UVoxel* InVoxel, bool bCheckOwner)
{
	return InVoxel && InVoxel->IsValidLowLevel() && InVoxel != EmptyVoxel && InVoxel != UnknownVoxel && (!bCheckOwner || InVoxel->Owner && InVoxel->Owner->IsValidLowLevel());
}

void UVoxel::Initialize(FIndex InIndex, AChunk* InOwner)
{
	if(!Owner || !Owner->IsValidLowLevel())
	{
		Index = InIndex;
		Owner = InOwner;
	}
}

void UVoxel::LoadData(const FString& InValue)
{
	TArray<FString> data;
	InValue.ParseIntoArray(data, TEXT(";"));

	Index = FIndex(data[0]);
	
	TArray<FString> rotData;
	data[1].ParseIntoArray(rotData, TEXT(","));
	Rotation = FRotator(FCString::Atof(*rotData[0]), FCString::Atof(*rotData[1]), FCString::Atof(*rotData[2]));
	
	TArray<FString> scaleData;
	data[2].ParseIntoArray(scaleData, TEXT(","));
	Scale = FVector(FCString::Atof(*scaleData[0]), FCString::Atof(*scaleData[1]), FCString::Atof(*scaleData[2]));
}

void UVoxel::LoadItem(const FVoxelItem& InVoxelItem)
{
	ID = InVoxelItem.ID;
	Index = InVoxelItem.Index;
	Rotation = InVoxelItem.Rotation;
	Scale = InVoxelItem.Scale;
	Owner = InVoxelItem.Owner;
	Auxiliary = InVoxelItem.Auxiliary;
}

FString UVoxel::ToData()
{
	return FString::Printf(TEXT("%s;%s;%s;%s"), *ID.ToString(), *Index.ToString(), *FString::Printf(TEXT("%f,%f,%f"), Rotation.Pitch, Rotation.Yaw, Rotation.Roll), *FString::Printf(TEXT("%f,%f,%f"), Scale.X, Scale.Y, Scale.Z));
}

FVoxelItem UVoxel::ToItem()
{
	FVoxelItem voxelItem;
	voxelItem.ID = ID;
	voxelItem.Index = Index;
	voxelItem.Rotation = Rotation;
	voxelItem.Scale = Scale;
	voxelItem.Owner = Owner;
	voxelItem.Auxiliary = Auxiliary;
	return voxelItem;
}

void UVoxel::OnSpawn_Implementation()
{
}

void UVoxel::OnDespawn_Implementation()
{
	ID = NAME_None;
	Index = FIndex::ZeroIndex;
	Rotation = FRotator::ZeroRotator;
	Scale = FVector::OneVector;
	Params.Empty();
	Owner = nullptr;
	Auxiliary = nullptr;
}

void UVoxel::OnTargetHit(ADWCharacter* InTarget, const FVoxelHitResult& InHitResult)
{
	
}

void UVoxel::OnTargetEnter(ADWCharacter* InTarget, const FVoxelHitResult& InHitResult)
{
	InTarget->SetOverlapVoxel(this);
}

void UVoxel::OnTargetStay(ADWCharacter* InTarget, const FVoxelHitResult& InHitResult)
{
	
}

void UVoxel::OnTargetExit(ADWCharacter* InTarget, const FVoxelHitResult& InHitResult)
{
	
}

bool UVoxel::OnMouseDown(EMouseButton InMouseButton, const FVoxelHitResult& InHitResult)
{
	if(!Owner || !Owner->IsValidLowLevel()) return false;

	switch (InMouseButton)
	{
		case EMouseButton::Left:
		{
			ADWPlayerCharacter* PlayerCharacter = UDWHelper::GetPlayerCharacter(this);
			if(PlayerCharacter)
			{
				PlayerCharacter->DestroyVoxel(InHitResult);
			}
			break;
		}
		case EMouseButton::Right:
		{
			ADWPlayerCharacter* PlayerCharacter = UDWHelper::GetPlayerCharacter(this);
			auto tmpSlot = UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryBar>(this)->GetSelectedSlot();
			if(PlayerCharacter && tmpSlot && !tmpSlot->IsEmpty())
			{
				FItem tmpItem = FItem::Clone(tmpSlot->GetItem(), 1);
				if(PlayerCharacter->GenerateVoxel(InHitResult, tmpItem))
				{
					tmpSlot->SubItem(tmpItem);
				}
			}
			break;
		}
		default: break;
	}
	return false;
}

bool UVoxel::OnMouseUp(EMouseButton InMouseButton, const FVoxelHitResult& InHitResult)
{
	return false;
}

bool UVoxel::OnMouseHold(EMouseButton InMouseButton, const FVoxelHitResult& InHitResult)
{
	return false;
}

void UVoxel::OnMouseHover(const FVoxelHitResult& InHitResult)
{
	
}

FVoxelData UVoxel::GetVoxelData() const
{
	return UDWHelper::LoadVoxelData(ID);
}
