// Fill out your copyright notice in the Description page of Project Settings.


#include "Voxel/VoxelPlant.h"
#include "World/Chunk.h"

UVoxelPlant::UVoxelPlant()
{

}

void UVoxelPlant::Initialize(FIndex InIndex, AChunk* InOwner)
{
	Super::Initialize(InIndex, InOwner);
}

bool UVoxelPlant::GetMeshDatas(TArray<FVector>& OutMeshVertices, TArray<FVector>& OutMeshNormals)
{
	FVector range = GetVoxelData().GetFinalRange(Rotation, Scale);

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

	return true;
}

bool UVoxelPlant::CheckAdjacent(EDirection InDirection)
{
	return true;
}

bool UVoxelPlant::OnMouseDown(EMouseButton InMouseButton, FVoxelHitResult InHitResult)
{
	switch (InMouseButton)
	{
		case EMouseButton::Left:
		{
			return Super::OnMouseDown(InMouseButton, InHitResult);
			break;
		}
	}
	return false;
}

bool UVoxelPlant::OnMouseUp(EMouseButton InMouseButton, FVoxelHitResult InHitResult)
{
	return Super::OnMouseUp(InMouseButton, InHitResult);
}

bool UVoxelPlant::OnMouseHold(EMouseButton InMouseButton, FVoxelHitResult InHitResult)
{
	return Super::OnMouseHold(InMouseButton, InHitResult);
}

void UVoxelPlant::OnMouseHover(FVoxelHitResult InHitResult)
{
	Super::OnMouseHover(InHitResult);
}

void UVoxelPlant::OnGenerate()
{
	Super::OnGenerate();
}

void UVoxelPlant::OnDestroy()
{
	Super::OnDestroy();
}

void UVoxelPlant::OnReplace()
{
	Super::OnReplace();
}

void UVoxelPlant::OnTargetHit(ADWCharacter* InTarget, FVoxelHitResult InHitResult)
{
	Super::OnTargetHit(InTarget, InHitResult);
}

void UVoxelPlant::OnTargetEnter(ADWCharacter* InTarget, FVoxelHitResult InHitResult)
{
	Super::OnTargetEnter(InTarget, InHitResult);
}

void UVoxelPlant::OnTargetStay(ADWCharacter* InTarget, FVoxelHitResult InHitResult)
{
	Super::OnTargetStay(InTarget, InHitResult);
}

void UVoxelPlant::OnTargetExit(ADWCharacter* InTarget, FVoxelHitResult InHitResult)
{
	Super::OnTargetExit(InTarget, InHitResult);
}