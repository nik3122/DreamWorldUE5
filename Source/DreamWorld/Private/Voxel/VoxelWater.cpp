// Fill out your copyright notice in the Description page of Project Settings.


#include "Voxel/VoxelWater.h"
#include "World/Chunk.h"
#include "World/WorldManager.h"
#include "Object.h"
#include "Character/DWCharacter.h"
#include "Character/Player/DWPlayerCharacter.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"

UVoxelWater::UVoxelWater()
{
	
}

void UVoxelWater::Initialize(FIndex InIndex, AChunk* InOwner)
{
	Super::Initialize(InIndex, InOwner);
}

void UVoxelWater::LoadData(const FString& InValue)
{
	Super::LoadData(InValue);
}

FString UVoxelWater::ToData()
{
	return Super::ToData();
}

void UVoxelWater::OnGenerate()
{
	Super::OnGenerate();
}

void UVoxelWater::OnDestroy()
{
	Super::OnDestroy();
}

void UVoxelWater::OnReplace()
{
	Super::OnReplace();
}

bool UVoxelWater::GetMeshDatas(TArray<FVector>& OutMeshVertices, TArray<FVector>& OutMeshNormals)
{
	return Super::GetMeshDatas(OutMeshVertices, OutMeshNormals);
}

bool UVoxelWater::CheckAdjacent(EDirection InDirection)
{
	return Super::CheckAdjacent(InDirection);
}

void UVoxelWater::OnTargetHit(ADWCharacter* InTarget, const FVoxelHitResult& InHitResult)
{
	Super::OnTargetHit(InTarget, InHitResult);
}

void UVoxelWater::OnTargetEnter(ADWCharacter* InTarget, const FVoxelHitResult& InHitResult)
{
	Super::OnTargetEnter(InTarget, InHitResult);
}

void UVoxelWater::OnTargetStay(ADWCharacter* InTarget, const FVoxelHitResult& InHitResult)
{
	Super::OnTargetStay(InTarget, InHitResult);

	UVoxel* Voxel = Owner->GetVoxel(Owner->LocationToIndex(InTarget->GetMesh()->GetSocketLocation(FName("Chest"))));
	if(UVoxel::IsValid(Voxel) && Voxel->GetVoxelData().VoxelType == EVoxelType::Water && InTarget->IsA(ADWPlayerCharacter::StaticClass()))
	{
		InTarget->Swim();
	}
	else
	{
		InTarget->UnSwim();
	}
}

void UVoxelWater::OnTargetExit(ADWCharacter* InTarget, const FVoxelHitResult& InHitResult)
{
	Super::OnTargetExit(InTarget, InHitResult);

	UVoxel* Voxel = Owner->GetVoxel(Owner->LocationToIndex(InTarget->GetMesh()->GetSocketLocation(FName("Chest"))));
	if(!UVoxel::IsValid(Voxel) || Voxel->GetVoxelData().VoxelType != EVoxelType::Water)
	{
		InTarget->UnSwim();
	}
}

bool UVoxelWater::OnMouseDown(EMouseButton InMouseButton, const FVoxelHitResult& InHitResult)
{
	return false;
}

bool UVoxelWater::OnMouseUp(EMouseButton InMouseButton, const FVoxelHitResult& InHitResult)
{
	return Super::OnMouseUp(InMouseButton, InHitResult);
}

bool UVoxelWater::OnMouseHold(EMouseButton InMouseButton, const FVoxelHitResult& InHitResult)
{
	return Super::OnMouseHold(InMouseButton, InHitResult);
}

void UVoxelWater::OnMouseHover(const FVoxelHitResult& InHitResult)
{
	Super::OnMouseHover(InHitResult);
}
