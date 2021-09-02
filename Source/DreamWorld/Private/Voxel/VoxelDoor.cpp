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

void UVoxelDoor::LoadData(const FString& InVlaue)
{
	Super::LoadData(InVlaue);
	
	TArray<FString> data;
	InVlaue.ParseIntoArray(data, TEXT(";"));
	
	bClosed = (bool)FCString::Atoi(*data[3]);
}

FString UVoxelDoor::ToData()
{
	return Super::ToData() + FString::Printf(TEXT(";%d"), (int)bClosed);
}

bool UVoxelDoor::CheckAdjacent(EDirection InDirection)
{
	return true;
}

bool UVoxelDoor::OnMouseDown(EMouseButton InMouseButton, FVoxelHitResult InHitResult)
{
	switch (InMouseButton)
	{
		case EMouseButton::Left:
		{
			return Super::OnMouseDown(InMouseButton, InHitResult);
			break;
		}
		case EMouseButton::Right:
		{
			FHitResult hitResult;
			if (!AWorldManager::GetCurrent()->VoxelTraceSingle(this, Owner->IndexToLocation(Index), hitResult))
			{
				OpenOrClose();
				return true;
			}
			break;
		}
	}
	return false;
}

bool UVoxelDoor::OnMouseUp(EMouseButton InMouseButton, FVoxelHitResult InHitResult)
{
	return Super::OnMouseUp(InMouseButton, InHitResult);
}

bool UVoxelDoor::OnMouseHold(EMouseButton InMouseButton, FVoxelHitResult InHitResult)
{
	return Super::OnMouseHold(InMouseButton, InHitResult);
}

void UVoxelDoor::OnMouseHover(FVoxelHitResult InHitResult)
{
	Super::OnMouseHover(InHitResult);
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

void UVoxelDoor::OnTargetHit(ADWCharacter* InTarget, FVoxelHitResult InHitResult)
{
	Super::OnTargetHit(InTarget, InHitResult);
}

void UVoxelDoor::OnTargetEnter(ADWCharacter* InTarget, FVoxelHitResult InHitResult)
{
	Super::OnTargetEnter(InTarget, InHitResult);
}

void UVoxelDoor::OnTargetStay(ADWCharacter* InTarget, FVoxelHitResult InHitResult)
{
	Super::OnTargetStay(InTarget, InHitResult);
}

void UVoxelDoor::OnTargetExit(ADWCharacter* InTarget, FVoxelHitResult InHitResult)
{
	Super::OnTargetExit(InTarget, InHitResult);
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
	Owner->GenerateMesh();
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
	Owner->GenerateMesh();
	if(GetVoxelData().OperationSounds.Num() > 1)
	{
		UGameplayStatics::PlaySoundAtLocation(this, GetVoxelData().OperationSounds[1], Owner->IndexToLocation(Index));
	}
	Owner->SetVoxelSample(Index, this);
}
