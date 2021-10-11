// Fill out your copyright notice in the Description page of Project Settings.


#include "Voxel/VoxelTorch.h"
#include "World/Chunk.h"
#include "World/WorldManager.h"

UVoxelTorch::UVoxelTorch()
{
	
}

void UVoxelTorch::LoadData(const FString& InValue)
{
	Super::LoadData(InValue);
}

FString UVoxelTorch::ToData()
{
	return Super::ToData();
}

void UVoxelTorch::OnTargetHit(ADWCharacter* InTarget, const FVoxelHitResult& InHitResult)
{
	Super::OnTargetHit(InTarget, InHitResult);
}

void UVoxelTorch::OnTargetEnter(UDWCharacterPart* InTarget, const FVoxelHitResult& InHitResult)
{
	Super::OnTargetEnter(InTarget, InHitResult);
}

void UVoxelTorch::OnTargetStay(UDWCharacterPart* InTarget, const FVoxelHitResult& InHitResult)
{
	Super::OnTargetStay(InTarget, InHitResult);
}

void UVoxelTorch::OnTargetExit(UDWCharacterPart* InTarget, const FVoxelHitResult& InHitResult)
{
	Super::OnTargetExit(InTarget, InHitResult);
}

bool UVoxelTorch::OnMouseDown(EMouseButton InMouseButton, const FVoxelHitResult& InHitResult)
{
	switch (InMouseButton)
	{
		case EMouseButton::Left:
		{
			return Super::OnMouseDown(InMouseButton, InHitResult);
		}
		default: break;
	}
	return false;
}

bool UVoxelTorch::OnMouseUp(EMouseButton InMouseButton, const FVoxelHitResult& InHitResult)
{
	return Super::OnMouseUp(InMouseButton, InHitResult);
}

bool UVoxelTorch::OnMouseHold(EMouseButton InMouseButton, const FVoxelHitResult& InHitResult)
{
	return Super::OnMouseHold(InMouseButton, InHitResult);
}

void UVoxelTorch::OnMouseHover(const FVoxelHitResult& InHitResult)
{
	Super::OnMouseHover(InHitResult);
}
