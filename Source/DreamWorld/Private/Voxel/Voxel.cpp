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
#include "VoxelAuxiliary/VoxelAuxiliary.h"
#include "Inventory/Character/CharacterInventory.h"
#include "Inventory/Slot/InventorySlot.h"
#include "SharedPointer.h"
#include "DataSaves/WorldDataSave.h"
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

UVoxel* UVoxel::NewVoxel(EVoxelType InVoxelType, UObject* InOuter)
{
	return NewVoxel(*FString::Printf(TEXT("Voxel_%d"), (int)InVoxelType), InOuter);
}

UVoxel* UVoxel::NewVoxel(const FName& InVoxelID, UObject* InOuter)
{
	FVoxelData voxelData = UDWHelper::LoadVoxelData(InVoxelID);
	TSubclassOf<UVoxel> tmpClass = UVoxel::StaticClass();
	if (voxelData.IsValid()) tmpClass = voxelData.VoxelClass;
	auto voxel = NewObject<UVoxel>(InOuter, tmpClass);
	voxel->VoxelID = InVoxelID;
	return voxel;
}

UVoxel* UVoxel::LoadVoxel(FVoxelItem VoxelItem, AChunk* InOwner)
{
	FString str1, str2;
	VoxelItem.VoxelData.Split(TEXT(";"), &str1, &str2);
	auto voxel = NewVoxel(FName(str1), InOwner);
	voxel->LoadData(str2);
	voxel->SetOwner(InOwner);
	voxel->SetAuxiliary(VoxelItem.VoxelAuxiliary);
	return voxel;
}

UVoxel* UVoxel::LoadVoxel(const FString& InVoxelData, AChunk* InOwner)
{
	FString str1, str2;
	InVoxelData.Split(TEXT(";"), &str1, &str2);
	auto voxel = NewVoxel(FName(str1), InOwner);
	voxel->LoadData(str2);
	voxel->SetOwner(InOwner);
	return voxel;
}

UVoxel* UVoxel::Empty()
{
	if (!EmptyVoxel || !EmptyVoxel->IsValidLowLevel())
	{
		EmptyVoxel = UVoxel::NewVoxel(EVoxelType::Empty, GetTransientPackage());
		EmptyVoxel->AddToRoot();
	}
	return EmptyVoxel;
}

UVoxel* UVoxel::Unknown()
{
	if (!UnknownVoxel || !UnknownVoxel->IsValidLowLevel())
	{
		UnknownVoxel = UVoxel::NewVoxel(EVoxelType::Unknown, GetTransientPackage());
		UnknownVoxel->AddToRoot();
	}
	return UnknownVoxel;
}

bool UVoxel::IsValid(UVoxel* InVoxel, bool bCheckOwner)
{
	return InVoxel && InVoxel->IsValidLowLevel() && InVoxel != Empty() && InVoxel != Unknown() && (!bCheckOwner || InVoxel->Owner && InVoxel->Owner->IsValidLowLevel());
}

void UVoxel::Initialize(FIndex InIndex, AChunk* InOwner)
{
	if(!Owner || !Owner->IsValidLowLevel())
	{
		Index = InIndex;
		Owner = InOwner;
	}
}

void UVoxel::LoadData(const FString& InVlaue)
{
	TArray<FString> data;
	InVlaue.ParseIntoArray(data, TEXT(";"));

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

	if (UVoxel::IsValid(adjacentVoxel))
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
							}
						}
						break;
					}
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
							}
						}
						break;
					}
				}
				break;
			}
		}
	}
	else if(adjacentVoxel == UVoxel::Unknown())
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

bool UVoxel::OnMouseDown(EMouseButton InMouseButton, FVoxelHitResult InHitResult)
{
	if(!Owner || !Owner->IsValidLowLevel()) return false;

	switch (InMouseButton)
	{
		case EMouseButton::Left:
		{
			if (GetVoxelData().VoxelType != EVoxelType::Bedrock)
			{
				return Owner->DestroyVoxel(this);
			}
			break;
		}
		case EMouseButton::Right:
		{
			FIndex index = Owner->LocationToIndex(InHitResult.Point - UDWHelper::GetWorldManager()->GetBlockSizedNormal(InHitResult.Normal)) + FIndex(InHitResult.Normal);
			UVoxel* voxel = Owner->GetVoxel(index);
			
			if(!IsValid(voxel) || (voxel->GetVoxelData().Transparency == ETransparency::Transparent && voxel != this))
			{
				auto tmpSlot = UDWHelper::GetWidgetInventoryBar()->GetSelectedSlot();
				if (!tmpSlot->IsEmpty() && tmpSlot->GetItem().GetData().Type == EItemType::Voxel)
				{
					UVoxel* tmpVoxel = NewVoxel(tmpSlot->GetItem().ID, Owner);

					//FRotator rotation = (Owner->VoxelIndexToLocation(index) + tmpVoxel->GetVoxelData().GetCeilRange() * 0.5f * UDWHelper::GetWorldManager()->GetBlockSize() - UDWHelper::GetPlayerCharacter()->GetActorLocation()).ToOrientationRotator();
					//rotation = FRotator(FRotator::ClampAxis(FMath::RoundToInt(rotation.Pitch / 90) * 90.f), FRotator::ClampAxis(FMath::RoundToInt(rotation.Yaw / 90) * 90.f), FRotator::ClampAxis(FMath::RoundToInt(rotation.Roll / 90) * 90.f));
					//tmpVoxel->Rotation = rotation;

					FHitResult hitResult;
					if (!UDWHelper::GetWorldManager()->VoxelTraceSingle(tmpVoxel, Owner->IndexToLocation(index), hitResult))
					{
						if (!IsValid(voxel) ? Owner->GenerateVoxel(index, tmpVoxel) : Owner->ReplaceVoxel(voxel, tmpVoxel))
						{
							tmpSlot->UseItem(1);
							return true;
						}
					}
				}
			}
			break;
		}
	}
	return false;
}

bool UVoxel::OnMouseUp(EMouseButton InMouseButton, FVoxelHitResult InHitResult)
{
	return false;
}

bool UVoxel::OnMouseHold(EMouseButton InMouseButton, FVoxelHitResult InHitResult)
{
	return false;	
}

void UVoxel::OnMouseHover(FVoxelHitResult InHitResult)
{
	
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
			if(Owner) location = Owner->IndexToLocation(Index, true) + GetVoxelData().GetCeilRange() * UDWHelper::GetWorldManager()->GetBlockSize() * 0.5f;
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
				if (UVoxel::IsValid(voxel))
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
						Owner->SetVoxelComplex(Index + FIndex(x, y, z), UVoxel::NewVoxel(EVoxelType::Water, Owner), true);
					}
				}
			}
		}
		if(GetVoxelData().GenerateSound) UGameplayStatics::PlaySoundAtLocation(this, GetVoxelData().GenerateSound, Owner->IndexToLocation(Index));
		Owner->SpawnPickUp(FItem(VoxelID), Owner->IndexToLocation(Index) + range * UDWHelper::GetWorldManager()->GetBlockSize() * 0.5f);
	}

	if (Auxiliary) Auxiliary->Destroy();
	
	if (IsRooted()) RemoveFromRoot();
	
	ConditionalBeginDestroy();
}

void UVoxel::OnReplace()
{
	OnDestroy();
}

void UVoxel::OnTargetHit(ADWCharacter* InTarget, FVoxelHitResult InHitResult)
{
	
}

void UVoxel::OnTargetEnter(ADWCharacter* InTarget, FVoxelHitResult InHitResult)
{
	InTarget->SetOverlapVoxel(this);
}

void UVoxel::OnTargetStay(ADWCharacter* InTarget, FVoxelHitResult InHitResult)
{
	if(InTarget == UDWHelper::GetPlayerCharacter())
	UDWHelper::Debug(GetVoxelData().Name.ToString());
}

void UVoxel::OnTargetExit(ADWCharacter* InTarget, FVoxelHitResult InHitResult)
{
	InTarget->SetOverlapVoxel(nullptr);
}

FVoxelData UVoxel::GetVoxelData() const
{
	return UDWHelper::LoadVoxelData(VoxelID);
}
