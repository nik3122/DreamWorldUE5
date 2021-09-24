// Fill out your copyright notice in the Description page of Project Settings.


#include "Voxel/VoxelDoor.h"
#include "World/Chunk.h"
#include "World/WorldManager.h"
#include "Kismet/GameplayStatics.h"

UVoxelDoor::UVoxelDoor()
{
	bClosed = true;
}

void UVoxelDoor::Initialize(FIndex InIndex, AChunk* InOwner)
{
	Super::Initialize(InIndex, InOwner);
}

void UVoxelDoor::LoadData(const FString& InValue)
{
	Super::LoadData(InValue);
	
	TArray<FString> data;
	InValue.ParseIntoArray(data, TEXT(";"));
	
	bClosed = (bool)FCString::Atoi(*data[3]);
}

FString UVoxelDoor::ToData()
{
	return Super::ToData() + FString::Printf(TEXT(";%d"), (int32)bClosed);
}

void UVoxelDoor::OnGenerate()
{
	Super::OnGenerate();
}

void UVoxelDoor::OnDestroy()
{
	Super::OnDestroy();
}

void UVoxelDoor::OnReplace()
{
	Super::OnReplace();
}

void UVoxelDoor::OnDespawn_Implementation()
{
	Super::OnDespawn_Implementation();
	bClosed = true;
}

bool UVoxelDoor::GetMeshDatas(TArray<FVector>& OutMeshVertices, TArray<FVector>& OutMeshNormals)
{
	return Super::GetMeshDatas(OutMeshVertices, OutMeshNormals);
}

bool UVoxelDoor::CheckAdjacent(EDirection InDirection)
{
	return true;
}

void UVoxelDoor::OpenOrClose()
{
	if(bClosed) OpenTheDoor();
	else CloseTheDoor();
}

void UVoxelDoor::OpenTheDoor()
{
	bClosed = false;
	Rotation += FRotator(0, -90, 0);
	Scale = FVector(1, 1, 1);
	Owner->Generate();
	if(GetVoxelData().OperationSounds.Num() > 0)
	{
		UGameplayStatics::PlaySoundAtLocation(this, GetVoxelData().OperationSounds[0], Owner->IndexToLocation(Index));
	}
	Owner->SetVoxelSample(Index, this);
}

void UVoxelDoor::CloseTheDoor()
{
	bClosed = true;
	Rotation += FRotator(0, 90, 0);
	Scale = FVector(1, 1, 1);
	Owner->Generate();
	if(GetVoxelData().OperationSounds.Num() > 1)
	{
		UGameplayStatics::PlaySoundAtLocation(this, GetVoxelData().OperationSounds[1], Owner->IndexToLocation(Index));
	}
	Owner->SetVoxelSample(Index, this);
}

void UVoxelDoor::OnTargetHit(ADWCharacter* InTarget, const FVoxelHitResult& InHitResult)
{
	Super::OnTargetHit(InTarget, InHitResult);
}

void UVoxelDoor::OnTargetEnter(ADWCharacter* InTarget, const FVoxelHitResult& InHitResult)
{
	Super::OnTargetEnter(InTarget, InHitResult);
}

void UVoxelDoor::OnTargetStay(ADWCharacter* InTarget, const FVoxelHitResult& InHitResult)
{
	Super::OnTargetStay(InTarget, InHitResult);
}

void UVoxelDoor::OnTargetExit(ADWCharacter* InTarget, const FVoxelHitResult& InHitResult)
{
	Super::OnTargetExit(InTarget, InHitResult);
}

bool UVoxelDoor::OnMouseDown(EMouseButton InMouseButton, const FVoxelHitResult& InHitResult)
{
	switch (InMouseButton)
	{
		case EMouseButton::Left:
		{
			return Super::OnMouseDown(InMouseButton, InHitResult);
		}
		case EMouseButton::Right:
		{
			FHitResult hitResult;
			if (!AWorldManager::Get()->VoxelTraceSingle(this, Owner->IndexToLocation(Index), hitResult))
			{
				OpenOrClose();
				return true;
			}
			break;
		}
	}
	return false;
}

bool UVoxelDoor::OnMouseUp(EMouseButton InMouseButton, const FVoxelHitResult& InHitResult)
{
	return Super::OnMouseUp(InMouseButton, InHitResult);
}

bool UVoxelDoor::OnMouseHold(EMouseButton InMouseButton, const FVoxelHitResult& InHitResult)
{
	return Super::OnMouseHold(InMouseButton, InHitResult);
}

void UVoxelDoor::OnMouseHover(const FVoxelHitResult& InHitResult)
{
	Super::OnMouseHover(InHitResult);
}
