#include "DWTypes.h"

#include "Chunk.h"
#include "ObjectPoolModuleBPLibrary.h"
#include "VoxelPlant.h"
#include "VoxelWater.h"
#include "World/WorldManager.h"
#include "Voxel/Voxel.h"
#include "Character/Player/DWPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

FItem FItem::Empty = FItem(NAME_None, 0, 0);

const FIndex FIndex::ZeroIndex = FIndex(0, 0, 0);

const FIndex FIndex::OneIndex = FIndex(1, 1, 1);

FTeamData FTeamData::Empty = FTeamData();

const FWorldData FWorldData::Empty = FWorldData();

FVoxelItem FVoxelItem::UnknownVoxel = FVoxelItem(FName("Unknown"));

FVoxelItem FVoxelItem::EmptyVoxel = FVoxelItem(FName("Empty"));

FItemData FItem::GetData() const
{
	return UDWHelper::LoadItemData(ID);
}

FSkillData FDWCharacterSkillAbilityData::GetItemData() const
{
	return UDWHelper::LoadSkillData(AbilityName);
}

UVoxel* FVoxelItem::GetVoxel() const
{
	return UVoxel::LoadVoxel(Owner, *this);
}

FVoxelData FVoxelItem::GetVoxelData() const
{
	return UDWHelper::LoadVoxelData(ID);
}

FString FVoxelItem::GetStringData() const
{
	FString tmpData;
	if(UVoxel* tmpVoxel = GetVoxel())
	{
		tmpData = tmpVoxel->ToData();
		UVoxel::DespawnVoxel(Owner, tmpVoxel);
	}
	return tmpData;
}

void FTeamData::AddMember(ADWCharacter* InMember)
{
	if (!Members.Contains(InMember))
	{
		InMember->SetTeamID(ID.ToString());
		Members.Add(InMember);
	}
}

void FTeamData::RemoveMember(ADWCharacter* InMember)
{
	if (Members.Contains(InMember))
	{
		if(!IsCaptain(InMember))
		{
			InMember->SetTeamID(TEXT(""));
			Members.Remove(InMember);
		}
		else
		{
			DissolveTeam();
		}
	}
}

void FTeamData::DissolveTeam()
{
	for (int i = 0; i < Members.Num(); i++)
	{
		if(Members[i] && Members[i]->IsValidLowLevel())
		{
			Members[i]->SetTeamID(TEXT(""));
		}
	}
	Members.Empty();
}

TArray<ADWCharacter*> FTeamData::GetMembers(ADWCharacter* InMember)
{
	auto tmpArr = TArray<ADWCharacter*>();
	for (int i = 0; i < Members.Num(); i++)
	{
		if (Members[i] != InMember)
			tmpArr.Add(Members[i]);
	}
	return tmpArr;
}

FVoxelHitResult::FVoxelHitResult()
{
	FMemory::Memzero(this, sizeof(FVoxelHitResult));
	Voxel = nullptr;
	Point = FVector();
	Normal = FVector();
}

FVoxelHitResult::FVoxelHitResult(UVoxel* InVoxel, FVector InPoint, FVector InNormal)
{
	FMemory::Memzero(this, sizeof(FVoxelHitResult));
	Voxel = InVoxel;
	Point = InPoint;
	Normal = InNormal;
}

AChunk* FVoxelHitResult::GetOwner() const
{
	if (Voxel.Get())
	{
		return Voxel.Get()->GetOwner();
	}
	return nullptr;
}

FVector FVoxelData::GetCeilRange(UVoxel* InVoxel) const
{
	return GetCeilRange(InVoxel->GetRotation(), InVoxel->GetScale());
}

FVector FVoxelData::GetFinalRange(UVoxel* InVoxel) const
{
	return GetFinalRange(InVoxel->GetRotation(), InVoxel->GetScale());
}

bool FVoxelData::GetMeshDatas(TArray<FVector>& OutMeshVertices, TArray<FVector>& OutMeshNormals, FRotator InRotation, FVector InScale) const
{
	if(VoxelClass == UVoxelPlant::StaticClass())
	{
		const FVector range = GetFinalRange(InRotation, InScale);

		OutMeshVertices = TArray<FVector>();
		OutMeshVertices.Add(FVector(-0.5f, -0.5f, -0.5f) * range);
		OutMeshVertices.Add(FVector(-0.5f, -0.5f, 0.5f) * range);
		OutMeshVertices.Add(FVector(0.5f, 0.5f, 0.5f) * range);
		OutMeshVertices.Add(FVector(0.5f, 0.5f, -0.5f) * range);
		OutMeshVertices.Add(FVector(0.5f, -0.5f, -0.5f) * range);
		OutMeshVertices.Add(FVector(0.5f, -0.5f, 0.5f) * range);
		OutMeshVertices.Add(FVector(-0.5f, 0.5f, 0.5f) * range);
		OutMeshVertices.Add(FVector(-0.5f, 0.5f, -0.5f) * range);

		OutMeshNormals = TArray<FVector>();
		OutMeshNormals.Add(FVector(1, -1, 0).GetSafeNormal());
		OutMeshNormals.Add(FVector(-1, -1, 0).GetSafeNormal());
	}
	else
	{
		OutMeshVertices = MeshVertices;
		OutMeshNormals = MeshNormals;
	}
	return OutMeshVertices.Num() > 0;
}

bool FDWGameplayEffectContainerSpec::HasValidTargets() const
{
	return TargetGameplayEffectSpecs.Num() > 0;
}

bool FDWGameplayEffectContainerSpec::HasValidEffects() const
{
	return TargetData.Num() > 0;
}

void FDWGameplayEffectContainerSpec::AddTargets(const TArray<FHitResult>& HitResults, const TArray<AActor*>& TargetActors)
{
	for (const FHitResult& HitResult : HitResults)
	{
		// 创建单个射线目标数据对象
		FGameplayAbilityTargetData_SingleTargetHit* NewData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
		// 将目标数据加入列表
		TargetData.Add(NewData);
	}

	if (TargetActors.Num() > 0)
	{
		// 创建目标Actor数组对象
		FGameplayAbilityTargetData_ActorArray* NewData = new FGameplayAbilityTargetData_ActorArray();
		NewData->TargetActorArray.Append(TargetActors);
		// 将目标数据加入列表
		TargetData.Add(NewData);
	}
}
