// Fill out your copyright notice in the Description page of Project Settings.


#include "Voxel/VoxelTorch.h"
#include "World/Chunk.h"
#include "World/WorldManager.h"

UVoxelTorch::UVoxelTorch()
{
	
}

void UVoxelTorch::Initialize(FIndex InIndex, AChunk* InOwner)
{
	Super::Initialize(InIndex, InOwner);
}

void UVoxelTorch::LoadData(const FString& InValue)
{
	Super::LoadData(InValue);
}

FString UVoxelTorch::ToData()
{
	return Super::ToData();
}

void UVoxelTorch::OnGenerate()
{
	Super::OnGenerate();
}

void UVoxelTorch::OnDestroy()
{
	Super::OnDestroy();
}

void UVoxelTorch::OnReplace()
{
	Super::OnReplace();
}

bool UVoxelTorch::GetMeshDatas(TArray<FVector>& OutMeshVertices, TArray<FVector>& OutMeshNormals)
{
	return Super::GetMeshDatas(OutMeshVertices, OutMeshNormals);
}

bool UVoxelTorch::CheckAdjacent(EDirection InDirection)
{
	return true;
}

void UVoxelTorch::OnTargetHit(ADWCharacter* InTarget, FVoxelHitResult InHitResult)
{
	Super::OnTargetHit(InTarget, InHitResult);
}

void UVoxelTorch::OnTargetEnter(ADWCharacter* InTarget, FVoxelHitResult InHitResult)
{
	Super::OnTargetEnter(InTarget, InHitResult);
}

void UVoxelTorch::OnTargetStay(ADWCharacter* InTarget, FVoxelHitResult InHitResult)
{
	Super::OnTargetStay(InTarget, InHitResult);
}

void UVoxelTorch::OnTargetExit(ADWCharacter* InTarget, FVoxelHitResult InHitResult)
{
	Super::OnTargetExit(InTarget, InHitResult);
}

bool UVoxelTorch::OnMouseDown(EMouseButton InMouseButton, FVoxelHitResult InHitResult)
{
	switch (InMouseButton)
	{
		case EMouseButton::Left:
		{
			return Super::OnMouseDown(InMouseButton, InHitResult);
		}
	}
	return false;
}

bool UVoxelTorch::OnMouseUp(EMouseButton InMouseButton, FVoxelHitResult InHitResult)
{
	return Super::OnMouseUp(InMouseButton, InHitResult);
}

bool UVoxelTorch::OnMouseHold(EMouseButton InMouseButton, FVoxelHitResult InHitResult)
{
	return Super::OnMouseHold(InMouseButton, InHitResult);
}

void UVoxelTorch::OnMouseHover(FVoxelHitResult InHitResult)
{
	Super::OnMouseHover(InHitResult);
}
