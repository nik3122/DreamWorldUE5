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

bool UVoxelWater::CheckAdjacent(EDirection InDirection)
{
	return Super::CheckAdjacent(InDirection);
}

bool UVoxelWater::OnMouseDown(EMouseButton InMouseButton, FVoxelHitResult InHitResult)
{
	return false;
}

bool UVoxelWater::OnMouseUp(EMouseButton InMouseButton, FVoxelHitResult InHitResult)
{
	return Super::OnMouseUp(InMouseButton, InHitResult);
}

bool UVoxelWater::OnMouseHold(EMouseButton InMouseButton, FVoxelHitResult InHitResult)
{
	return Super::OnMouseHold(InMouseButton, InHitResult);
}

void UVoxelWater::OnMouseHover(FVoxelHitResult InHitResult)
{
	Super::OnMouseHover(InHitResult);
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

void UVoxelWater::OnTargetHit(ADWCharacter* InTarget, FVoxelHitResult InHitResult)
{
	Super::OnTargetHit(InTarget, InHitResult);
}

void UVoxelWater::OnTargetEnter(ADWCharacter* InTarget, FVoxelHitResult InHitResult)
{
	Super::OnTargetEnter(InTarget, InHitResult);
}

void UVoxelWater::OnTargetStay(ADWCharacter* InTarget, FVoxelHitResult InHitResult)
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

void UVoxelWater::OnTargetExit(ADWCharacter* InTarget, FVoxelHitResult InHitResult)
{
	Super::OnTargetExit(InTarget, InHitResult);

	UVoxel* Voxel = Owner->GetVoxel(Owner->LocationToIndex(InTarget->GetMesh()->GetSocketLocation(FName("Chest"))));
	if(!UVoxel::IsValid(Voxel) || Voxel->GetVoxelData().VoxelType != EVoxelType::Water)
	{
		InTarget->UnSwim();
	}
}
