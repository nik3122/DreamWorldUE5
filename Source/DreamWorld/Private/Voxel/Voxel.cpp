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
	VoxelID = NAME_None;
	Index = FIndex::ZeroIndex;
	Rotation = FRotator::ZeroRotator;
	Scale = FVector::OneVector;
	Owner = nullptr;
	Auxiliary = nullptr;
}

UVoxel* UVoxel::NewVoxel(UObject* InWorldContext, EVoxelType InVoxelType)
{
	return NewVoxel(InWorldContext, *FString::Printf(TEXT("Voxel_%d"), (int32)InVoxelType));
}

UVoxel* UVoxel::NewVoxel(UObject* InWorldContext, const FName& InVoxelID)
{
	FVoxelData voxelData = UDWHelper::LoadVoxelData(InVoxelID);
	TSubclassOf<UVoxel> tmpClass = StaticClass();
	if (voxelData.IsValid()) tmpClass = voxelData.VoxelClass;
	auto voxel = UObjectPoolModuleBPLibrary::SpawnObject<UVoxel>(InWorldContext, tmpClass);
	voxel->VoxelID = InVoxelID;
	return voxel;
}

UVoxel* UVoxel::LoadVoxel(AChunk* InOwner, FVoxelItem InVoxelItem)
{
	FString str1, str2;
	InVoxelItem.VoxelData.Split(TEXT(";"), &str1, &str2);
	auto voxel = NewVoxel(InOwner, *str1);
	voxel->LoadData(str2);
	voxel->SetOwner(InOwner);
	voxel->SetAuxiliary(InVoxelItem.VoxelAuxiliary);
	return voxel;
}

UVoxel* UVoxel::LoadVoxel(AChunk* InOwner, const FString& InVoxelData)
{
	FString str1, str2;
	InVoxelData.Split(TEXT(";"), &str1, &str2);
	auto voxel = NewVoxel(InOwner, *str1);
	voxel->LoadData(str2);
	voxel->SetOwner(InOwner);
	return voxel;
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

FString UVoxel::ToData()
{
	return FString::Printf(TEXT("%s;%s;%s;%s"), *VoxelID.ToString(), *Index.ToString(), *FString::Printf(TEXT("%f,%f,%f"), Rotation.Pitch, Rotation.Yaw, Rotation.Roll), *FString::Printf(TEXT("%f,%f,%f"), Scale.X, Scale.Y, Scale.Z));
}

void UVoxel::OnGenerate()
{
	if ((!Auxiliary || Auxiliary->IsValidLowLevel()) && GetVoxelData().AuxiliaryClass != nullptr)
	{
		Auxiliary = GWorld->SpawnActor<AVoxelAuxiliary>(GetVoxelData().AuxiliaryClass);
		if (Auxiliary != nullptr)
		{
			Auxiliary->AttachToActor(Cast<AActor>(GetOuter()), FAttachmentTransformRules::KeepRelativeTransform);
			FVector location = FVector::ZeroVector;
			if(Owner) location = Owner->IndexToLocation(Index, true) + GetVoxelData().GetCeilRange() * AWorldManager::GetData().BlockSize * 0.5f;
			Auxiliary->Initialize(this, location);
			Owner->SetVoxelSample(Index, this);
		}
	}
}

void UVoxel::OnDestroy()
{
	if (Owner && Owner->IsValidLowLevel())
	{
		FVector range = GetVoxelData().GetCeilRange(this);
		for (int x = 0; x < range.X; x++)
		{
			for (int y = 0; y < range.Y; y++)
			{
				FIndex index = Index + FIndex(x, y, range.Z);
				UVoxel* voxel = Owner->GetVoxel(index);
				if (IsValid(voxel))
				{
					FVoxelData voxelData = voxel->GetVoxelData();
					if(voxelData.Transparency == ETransparency::Transparent && voxelData.VoxelType != EVoxelType::Water)
					{
						Owner->DestroyVoxel(index);
					}
				}
			}
		}
		if (CheckNeighbors(EVoxelType::Water, true))
		{
			for (int x = 0; x < range.X; x++)
			{
				for (int y = 0; y < range.Y; y++)
				{
					for (int z = 0; z < range.Z; z++)
					{
						Owner->SetVoxelComplex(Index + FIndex(x, y, z), NewVoxel(this, EVoxelType::Water), true);
					}
				}
			}
		}
		if(GetVoxelData().GenerateSound) UGameplayStatics::PlaySoundAtLocation(this, GetVoxelData().GenerateSound, Owner->IndexToLocation(Index));
		Owner->SpawnPickUp(FItem(VoxelID, 1), Owner->IndexToLocation(Index) + range * AWorldManager::GetData().BlockSize * 0.5f);
	}

	if (Auxiliary) Auxiliary->Destroy();
	
	if (IsRooted()) RemoveFromRoot();
	
	ConditionalBeginDestroy();
}

void UVoxel::OnReplace()
{
	OnDestroy();
}

void UVoxel::OnSpawn_Implementation()
{
}

void UVoxel::OnDespawn_Implementation()
{
	VoxelID = NAME_None;
	Index = FIndex::ZeroIndex;
	Rotation = FRotator::ZeroRotator;
	Scale = FVector::OneVector;
	Owner = nullptr;
	Auxiliary = nullptr;
}

bool UVoxel::GetMeshDatas(TArray<FVector>& OutMeshVertices, TArray<FVector>& OutMeshNormals)
{
	if (GetVoxelData().MeshVertices.Num() > 0)
	{
		OutMeshVertices = GetVoxelData().MeshVertices;
		OutMeshNormals = GetVoxelData().MeshNormals;
		return true;
	}
	return false;
}

bool UVoxel::CheckAdjacent(EDirection InDirection)
{
	if (Owner == nullptr || !Owner->IsValidLowLevel()) return true;

	if(InDirection == EDirection::Down && Owner->LocalIndexToWorld(Index).Z == 0) return false;

	UVoxel* adjacentVoxel = Owner->GetVoxel(UDWHelper::GetAdjacentIndex(Index, InDirection, Rotation));

	if (IsValid(adjacentVoxel))
	{
		FVoxelData voxelData = GetVoxelData();
		FVoxelData adjacentData = adjacentVoxel->GetVoxelData();
		switch (voxelData.Transparency)
		{
			case ETransparency::Solid:
			{
				switch (adjacentData.Transparency)
				{
					case ETransparency::Solid:
					{
						return false;
					}
					default: break;
				}
				break;
			}
			case ETransparency::SemiTransparent:
			{
				switch (adjacentData.Transparency)
				{
					case ETransparency::SemiTransparent:
					{
						if (voxelData.VoxelType == adjacentData.VoxelType)
						{
							switch (voxelData.VoxelType)
							{
								case EVoxelType::Oak_Leaves:
								case EVoxelType::Birch_Leaves:
								case EVoxelType::Ice:
								case EVoxelType::Glass:
								{
									return false;
								}
								default: break;
							}
						}
						break;
					}
					default: break;
				}
				break;
			}
			case ETransparency::Transparent:
			{
				switch (adjacentData.Transparency)
				{
					case ETransparency::Solid:
					case ETransparency::SemiTransparent:
					{
						return false;
					}
					case ETransparency::Transparent:
					{
						if (voxelData.VoxelType == adjacentData.VoxelType)
						{
							switch (voxelData.VoxelType)
							{
								case EVoxelType::Water:
								{
									return false;
								}
								default: break;
							}
						}
						break;
					}
				}
				break;
			}
		}
	}
	else if(adjacentVoxel == UnknownVoxel)
	{
		return false;
	}
	return true;
}

bool UVoxel::CheckNeighbors(EVoxelType InVoxelType, bool bIgnoreBottom /*= false*/, int InDistance /*= 1*/)
{
	FVector range = GetVoxelData().GetCeilRange(this);
	for (int x = -InDistance; x < range.X + InDistance; x++)
	{
		for (int y = -InDistance; y < range.Y + InDistance; y++)
		{
			for (int z = bIgnoreBottom ? 0 : -InDistance; z < range.Z + InDistance; z++)
			{
				UVoxel* voxel = Owner->GetVoxel(Index + FIndex(x, y, z));
				if (UVoxel::IsValid(voxel))
				{
					if (voxel->GetVoxelData().VoxelType == InVoxelType)
					{
						return true;
					}
				}
			}
		}
	}
	return false;
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
	//if(InTarget == UDWHelper::GetPlayerCharacter(this))
	//UDWHelper::Debug(GetVoxelData().Name.ToString());
}

void UVoxel::OnTargetExit(ADWCharacter* InTarget, const FVoxelHitResult& InHitResult)
{
	InTarget->SetOverlapVoxel(nullptr);
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
	return UDWHelper::LoadVoxelData(VoxelID);
}
