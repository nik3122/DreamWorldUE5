// Fill out your copyright notice in the Description page of Project Settings.PickUp/


#include "World/Chunk.h"
#include "Voxel/Voxel.h"
#include "PickUp/PickUp.h"
#include "PickUp/PickUpVoxel.h"
#include "Components/BoxComponent.h"
#include "Character/Player/DWPlayerCharacter.h"
#include "World/WorldManager.h"
#include "Voxel/Components/VoxelMeshComponent.h"
#include "Vitality/VitalityObject.h"
#include "DWGameInstance.h"
#include "DataSaves/WorldDataSave.h"
#include "Engine/World.h"
#include "PickUp/PickUpVoxel.h"
#include "Character/DWCharacter.h"
#include "Engine/World.h"
#include "PickUp/PickUpEquip.h"
#include "PickUp/PickUpProp.h"
#include "Kismet/GameplayStatics.h"
#include "VoxelAuxiliary/VoxelAuxiliary.h"

// Sets default values
AChunk::AChunk()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	SolidMesh = nullptr;
	TransMesh = nullptr;

	Batch = 0;
	Index = FVector();
	bGenerated = false;
	VoxelMap = TMap<FIndex, UVoxel*>();
	Neighbors = TArray<AChunk*>();
	for (int32 i = 0; i < 6; i++)
	{
		Neighbors.Add(nullptr);
	}
	Characters = TArray<ADWCharacter*>();
	VitalityObjects = TArray<AVitalityObject*>();
	PickUps = TArray<APickUp*>();
}

// Called when the game starts or when spawned
void AChunk::BeginPlay()
{
	Super::BeginPlay();

}

void AChunk::OnCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor->IsA(ADWPlayerCharacter::StaticClass()))
	{
		//FIndex index = LocationToVoxelIndex(Hit.ImpactPoint - UDWHelper::GetWorldManager()->GetBlockSizedNormal(Hit.ImpactNormal, 0.01f));
		//UVoxel* voxel = GetVoxel(index);
		//if (UVoxel::IsValid(voxel))
		//{
		//	voxel->OnTargetHit(Cast<ADWPlayerCharacter>(OtherActor), FVoxelHitResult(voxel, Hit.ImpactPoint, Hit.ImpactNormal));
		//}
	}
}

void AChunk::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(ADWPlayerCharacter::StaticClass()))
	{
		//auto character = Cast<ADWPlayerCharacter>(OtherActor);
		//UVoxel* voxel = GetVoxel(LocationToVoxelIndex(SweepResult.ImpactPoint + UDWHelper::GetWorldManager()->GetBlockSizedNormal(SweepResult.ImpactNormal)));
		//if (UVoxel::IsValid(voxel))
		//{
		//	if (character->GetOverlapVoxel())
		//	{
		//		if(character->GetOverlapVoxel()->GetVoxelData().VoxelType != EVoxelType::Water)
		//		{
		//			character->GetOverlapVoxel()->OnTargetExit(character);
		//		}
		//		else if(!UVoxel::IsValid(GetVoxel(LocationToVoxelIndex(character->GetActorLocation()))))
		//		{
		//			character->GetOverlapVoxel()->OnTargetExit(character);
		//		}
		//	}
		//	voxel->OnTargetEnter(character, FVoxelHitResult(voxel, SweepResult.ImpactPoint, SweepResult.ImpactNormal));
		//}
	}
}

void AChunk::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA(ADWPlayerCharacter::StaticClass()))
	{
		//auto character = Cast<ADWPlayerCharacter>(OtherActor);
		//UVoxel* voxel = GetVoxel(LocationToVoxelIndex(character->GetActorLocation()));
		//if (UVoxel::IsValid(voxel))
		//{
		//	voxel->OnTargetEnter(character, FVoxelHitResult(voxel, character->GetActorLocation(), character->GetMoveDirection()));
		//}
		//else if(character->GetOverlapVoxel())
		//{
		//	character->GetOverlapVoxel()->OnTargetExit(character);
		//}
	}
}

// Called every frame
void AChunk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AChunk::Initialize(FIndex InIndex, int32 InBatch)
{
	Index = InIndex;
	Batch = InBatch;
	GetNeighbors();
}

void AChunk::OnGenerated()
{
	bGenerated = true;

	for (int32 i = 0; i < Neighbors.Num(); i++)
	{
		if (Neighbors[i] && Neighbors[i]->Batch != Batch)
		{
			Neighbors[i]->GenerateMesh();
		}
	}

	for (auto iter = VoxelMap.CreateIterator(); iter; ++iter)
	{
		UVoxel* voxel = iter->Value;
		if(UVoxel::IsValid(voxel))
		{
			voxel->OnGenerate();
		}
	}

	if (UDWHelper::GetWorldDataSave()->IsExistChunkData(Index))
	{
		LoadActors(UDWHelper::GetWorldDataSave()->LoadChunkData(Index));
	}
	else
	{
		if (FMath::FRand() <= UDWHelper::GetWorldManager()->GetVitalityRaceDensity())
		{
			auto raceData = UDWHelper::RandomVitalityRaceData();
			auto tmpNum = FMath::RandRange(raceData.MinNum, raceData.MaxNum);
			for (int32 j = 0; j < tmpNum; j++)
			{
				FHitResult hitResult;
				auto raceItem = raceData.RaceItems[FMath::RandRange(0, raceData.RaceItems.Num() - 1)];
				if (UDWHelper::GetWorldManager()->ChunkTraceSingle(this, FMath::Max(raceItem.Range.X, raceItem.Range.Y) * 0.5f * UDWHelper::GetWorldManager()->GetBlockSize(), raceItem.Range.Z * 0.5f * UDWHelper::GetWorldManager()->GetBlockSize(), hitResult))
				{
					auto saveData = FVitalityObjectData();
					saveData.Class = raceItem.Class;
					saveData.RaceID = raceData.ID.ToString();
					saveData.Location = hitResult.Location;
					saveData.Rotation = FRotator(0, FMath::RandRange(0, 360), 0);
					SpawnVitalityObject(saveData);
				}
			}
		}
		if (FMath::FRand() <= UDWHelper::GetWorldManager()->GetCharacterRaceDensity())
		{
			ADWCharacter* captain = nullptr;
			auto raceData = UDWHelper::RandomCharacterRaceData();
			auto tmpNum = FMath::RandRange(raceData.MinNum, raceData.MaxNum);
			for (int32 j = 0; j < tmpNum; j++)
			{
				FHitResult hitResult;
				auto raceItem = raceData.RaceItems[FMath::RandRange(0, raceData.RaceItems.Num() - 1)];
				if (UDWHelper::GetWorldManager()->ChunkTraceSingle(this, FMath::Max(raceItem.Range.X, raceItem.Range.Y) * 0.5f * UDWHelper::GetWorldManager()->GetBlockSize(), raceItem.Range.Z * 0.5f * UDWHelper::GetWorldManager()->GetBlockSize(), hitResult))
				{
					auto saveData = FCharacterData();
					saveData.Class = raceItem.Class;
					saveData.RaceID = raceData.ID.ToString();
					saveData.Location = hitResult.Location;
					saveData.Rotation = FRotator(0, FMath::RandRange(0, 360), 0);

					auto character = SpawnCharacter(saveData);

					if (character != nullptr)
					{
						if (captain == nullptr)
						{
							captain = character;
							UDWHelper::GetWorldManager()->CreateTeam(captain);
						}
						else
						{
							captain->AddTeamMate(character);
						}
					}
				}
			}
		}
	}
}

void AChunk::OnDestroy()
{
	if (bGenerated)
	{
		FChunkData chunkData = FChunkData();

		chunkData.Index = Index;
		chunkData.bInitialized = true;

		for (auto iter = VoxelMap.CreateConstIterator(); iter; ++iter)
		{
			UVoxel* voxel = iter->Value;
			chunkData.VoxelDatas.Add(voxel->ToData());
			if (voxel->GetAuxiliary())
			{
				voxel->GetAuxiliary()->ConditionalBeginDestroy();
			}
		}
		for (int32 i = 0; i < PickUps.Num(); i++)
		{
			chunkData.PickUpDatas.Add(PickUps[i]->ToData());
		}
		for (int32 i = 0; i < Characters.Num(); i++)
		{
			if (IsOnTheChunk(Characters[i]->GetActorLocation()))
			{
				chunkData.CharacterDatas.Add(Characters[i]->ToData());
			}
		}
		for (int32 i = 0; i < VitalityObjects.Num(); i++)
		{
			chunkData.VitalityObjectDatas.Add(VitalityObjects[i]->ToData());
		}

		UDWHelper::GetWorldDataSave()->SaveChunkData(Index, chunkData);

		for (int32 i = 0; i < PickUps.Num(); i++)
		{
			PickUps[i]->Destroy();
		}

		for (int32 i = 0; i < Characters.Num(); i++)
		{
			if (IsOnTheChunk(Characters[i]->GetActorLocation()))
			{
				Characters[i]->Destroy();
			}
			else
			{
				AChunk* chunk = UDWHelper::GetWorldManager()->FindChunk(Characters[i]->GetActorLocation());
				if (chunk && chunk->IsValidLowLevel())
				{
					chunk->AttachCharacter(Characters[i]);
				}
			}
		}

		for (int32 i = 0; i < VitalityObjects.Num(); i++)
		{
			VitalityObjects[i]->Destroy();
		}
	}

	BreakNeighbors();

	Destroy();
}

UVoxel* AChunk::GetVoxel(FIndex InIndex)
{
	return GetVoxel(InIndex.X, InIndex.Y, InIndex.Z);
}

UVoxel* AChunk::GetVoxel(int InX, int InY, int InZ)
{
	if (InX < 0) {
		if (Neighbors[(int)EDirection::Back] != nullptr)
			return Neighbors[(int)EDirection::Back]->GetVoxel(InX + UDWHelper::GetWorldManager()->GetChunkSize(), InY, InZ);
		return UVoxel::Unknown();
	}
	else if (InX >= UDWHelper::GetWorldManager()->GetChunkSize()) {
		if (Neighbors[(int)EDirection::Forward] != nullptr)
			return Neighbors[(int)EDirection::Forward]->GetVoxel(InX - UDWHelper::GetWorldManager()->GetChunkSize(), InY, InZ);
		return UVoxel::Unknown();
	}
	else if (InY < 0) {
		if (Neighbors[(int)EDirection::Left] != nullptr)
			return Neighbors[(int)EDirection::Left]->GetVoxel(InX, InY + UDWHelper::GetWorldManager()->GetChunkSize(), InZ);
		return UVoxel::Unknown();
	}
	else if (InY >= UDWHelper::GetWorldManager()->GetChunkSize()) {
		if (Neighbors[(int)EDirection::Right] != nullptr)
			return Neighbors[(int)EDirection::Right]->GetVoxel(InX, InY - UDWHelper::GetWorldManager()->GetChunkSize(), InZ);
		return UVoxel::Unknown();
	}
	else if (InZ < 0) {
		if (Neighbors[(int)EDirection::Down] != nullptr)
			return Neighbors[(int)EDirection::Down]->GetVoxel(InX, InY, InZ + UDWHelper::GetWorldManager()->GetChunkSize());
		else if (Index.Z > 0)
			return UVoxel::Unknown();
	}
	else if (InZ >= UDWHelper::GetWorldManager()->GetChunkSize()) {
		if (Neighbors[(int)EDirection::Up] != nullptr)
			return Neighbors[(int)EDirection::Up]->GetVoxel(InX, InY, InZ - UDWHelper::GetWorldManager()->GetChunkSize());
		else if (Index.Z < UDWHelper::GetWorldManager()->GetChunkHightRange())
			return UVoxel::Unknown();
	}
	else if (VoxelMap.Contains(FIndex(InX, InY, InZ))) {
		return VoxelMap[FIndex(InX, InY, InZ)];
	}
	return UVoxel::Empty();
}

bool AChunk::CheckVoxel(FIndex InIndex, FVector InRange/* = FVector::OneVector*/, bool bIgnoreTransparent /*= true*/)
{
	if (InRange != FVector::OneVector)
	{
		for (int x = 0; x < InRange.X; x++)
		{
			for (int y = 0; y < InRange.Y; y++)
			{
				for (int z = 0; z < InRange.Z; z++)
				{
					UVoxel* voxel = GetVoxel(InIndex + FIndex(x, y, z));
					if (UVoxel::IsValid(voxel) && (!bIgnoreTransparent || voxel->GetVoxelData().Transparency != ETransparency::Transparent))
					{
						return true;
					}
				}
			}
		}
	}
	else
	{
		UVoxel* voxel = GetVoxel(InIndex);
		if (UVoxel::IsValid(voxel) && (!bIgnoreTransparent || voxel->GetVoxelData().Transparency != ETransparency::Transparent))
		{
			return true;
		}
	}
	return false;
}

bool AChunk::CheckVoxel(int InX, int InY, int InZ, FVector InRange/* = FVector::OneVector*/, bool bIgnoreTransparent /*= true*/)
{
	return CheckVoxel(FIndex(InX, InY, InZ), InRange, bIgnoreTransparent);
}

bool AChunk::SetVoxelSample(FIndex InIndex, UVoxel* InVoxel, bool bGenerateMesh /*= false*/)
{
	bool tmpBool = false;

	if (UVoxel::IsValid(InVoxel, false))
	{
		if (!CheckVoxel(InIndex))
		{
			if (!VoxelMap.Contains(InIndex))
			{
				VoxelMap.Add(InIndex, InVoxel);
			}
			else
			{
				UVoxel* voxel = GetVoxel(InIndex);
				if (bGenerateMesh && UVoxel::IsValid(voxel))
				{
					voxel->OnDestroy();
				}
				VoxelMap[InIndex] = InVoxel;
			}
			InVoxel->Initialize(InIndex, this);
			tmpBool = true;
		}
	}
	else if (VoxelMap.Contains(InIndex))
	{
		VoxelMap.Remove(InIndex);
		tmpBool = true;
	}

	if (tmpBool && bGenerateMesh)
	{
		GenerateMesh();
		UpdateNeighbors(InIndex);
	}

	return tmpBool;
}

bool AChunk::SetVoxelSample(int InX, int InY, int InZ, UVoxel* InVoxel, bool bGenerateMesh /*= false*/)
{
	return SetVoxelSample(FIndex(InX, InY, InZ), InVoxel, bGenerateMesh);
}

bool AChunk::SetVoxelComplex(FIndex InIndex, UVoxel* InVoxel, bool bGenerateMesh /*= false*/)
{
	return SetVoxelComplex(InIndex.X, InIndex.Y, InIndex.Z, InVoxel, bGenerateMesh);
}

bool AChunk::SetVoxelComplex(int InX, int InY, int InZ, UVoxel* InVoxel, bool bGenerateMesh /*= false*/)
{
	if (InX < 0) {
		if (Neighbors[(int)EDirection::Back] != nullptr)
			return Neighbors[(int)EDirection::Back]->SetVoxelComplex(InX + UDWHelper::GetWorldManager()->GetChunkSize(), InY, InZ, InVoxel, bGenerateMesh);
	}
	else if (InX >= UDWHelper::GetWorldManager()->GetChunkSize()) {
		if (Neighbors[(int)EDirection::Forward] != nullptr)
			return Neighbors[(int)EDirection::Forward]->SetVoxelComplex(InX - UDWHelper::GetWorldManager()->GetChunkSize(), InY, InZ, InVoxel, bGenerateMesh);
	}
	else if (InY < 0) {
		if (Neighbors[(int)EDirection::Left] != nullptr)
			return Neighbors[(int)EDirection::Left]->SetVoxelComplex(InX, InY + UDWHelper::GetWorldManager()->GetChunkSize(), InZ, InVoxel, bGenerateMesh);
	}
	else if (InY >= UDWHelper::GetWorldManager()->GetChunkSize()) {
		if (Neighbors[(int)EDirection::Right] != nullptr)
			return Neighbors[(int)EDirection::Right]->SetVoxelComplex(InX, InY - UDWHelper::GetWorldManager()->GetChunkSize(), InZ, InVoxel, bGenerateMesh);
	}
	else if (InZ < 0) {
		if (Neighbors[(int)EDirection::Down] != nullptr)
			return Neighbors[(int)EDirection::Down]->SetVoxelComplex(InX, InY, InZ + UDWHelper::GetWorldManager()->GetChunkSize(), InVoxel, bGenerateMesh);
	}
	else if (InZ >= UDWHelper::GetWorldManager()->GetChunkSize()) {
		if (Neighbors[(int)EDirection::Up] == nullptr)
			UDWHelper::GetWorldManager()->SpawnChunk(Index + UDWHelper::DirectionToIndex(EDirection::Up), !bGenerateMesh);
		if (Neighbors[(int)EDirection::Up] != nullptr)
			return Neighbors[(int)EDirection::Up]->SetVoxelComplex(InX, InY, InZ - UDWHelper::GetWorldManager()->GetChunkSize(), InVoxel, bGenerateMesh);
	}
	else {
		if (InVoxel->GetVoxelData().IsComplex())
		{
			if (InVoxel != UVoxel::Empty())
			{
				FVector range = InVoxel->GetVoxelData().GetCeilRange(InVoxel);
				if (!CheckVoxel(InX, InY, InZ, range))
				{
					for (int x = 0; x < range.X; x++)
					{
						for (int y = 0; y < range.Y; y++)
						{
							for (int z = 0; z < range.Z; z++)
							{
								FIndex index = FIndex(InX + x, InY + y, InZ + z);
								SetVoxelSample(index, InVoxel, bGenerateMesh);
							}
						}
					}
					return true;
				}
			}
			else
			{
				UVoxel* voxel = GetVoxel(InX, InY, InZ);
				if(UVoxel::IsValid(voxel))
				{
					FVector range = voxel->GetVoxelData().GetCeilRange(voxel);
					for (int x = 0; x < range.X; x++)
					{
						for (int y = 0; y < range.Y; y++)
						{
							for (int z = 0; z < range.Z; z++)
							{
								FIndex index = voxel->GetIndex() + FIndex(InX, InY, InZ);
								if (VoxelMap.Contains(index))
								{
									VoxelMap.Remove(index);
								}
							}
						}
					}
					return true;
				}
			}
		}
		else
		{
			return SetVoxelSample(InX, InY, InZ, InVoxel, bGenerateMesh);
		}
	}
	return false;
}

void AChunk::GetNeighbors()
{
	for (int i = 0; i < 6; i++)
	{
		Neighbors[i] = UDWHelper::GetWorldManager()->FindChunk(Index + UDWHelper::DirectionToIndex((EDirection)i));
		if (Neighbors[i] != nullptr)
		{
			Neighbors[i]->Neighbors[(int)UDWHelper::InvertDirection((EDirection)i)] = this;
		}
	}
}

void AChunk::BreakNeighbors()
{
	for (int i = 0; i < 6; i++)
	{
		if (Neighbors[i] != nullptr)
		{
			Neighbors[i]->Neighbors[(int)UDWHelper::InvertDirection((EDirection)i)] = nullptr;
			Neighbors[i] = nullptr;
		}
	}
}

void AChunk::BuildMap()
{
	for (int x = 0; x < UDWHelper::GetWorldManager()->GetChunkSize(); x++)
	{
		for (int y = 0; y < UDWHelper::GetWorldManager()->GetChunkSize(); y++)
		{
			for (int z = 0; z < UDWHelper::GetWorldManager()->GetChunkSize(); z++)
			{
				FIndex voxelIndex = FIndex(x, y, z);

				if (VoxelMap.Contains(voxelIndex)) continue;

				EVoxelType voxelType = UDWHelper::GetWorldManager()->GetNoiseVoxelType(LocalIndexToWorld(voxelIndex));

				if (voxelType != EVoxelType::Empty)
				{
					switch (voxelType)
					{
						// grass
						case EVoxelType::Grass:
						{
							float tmpNum = FMath::FRandRange(0, 1);
							// plant
							if (tmpNum < 0.2f)
							{
								SetVoxelComplex(FIndex(x, y, z + 1), UVoxel::NewVoxel(FMath::FRandRange(0, 1) > 0.2f ? EVoxelType::Tall_Grass : (EVoxelType)FMath::RandRange((int)EVoxelType::Flower_Allium, (int)EVoxelType::Flower_Tulip_White), this));
							}
							// tree
							else if (tmpNum < 0.21f)
							{
								if ((x > 2 && x <= UDWHelper::GetWorldManager()->GetChunkSize() - 2) && (y > 2 && y <= UDWHelper::GetWorldManager()->GetChunkSize() - 2))
								{
									int treeHight = FMath::RandRange(4, 5);
									int leavesHight = 2/*FMath::RandRange(2, 2)*/;
									int leavesWidth = FMath::FRandRange(0, 1) < 0.5f ? 3 : 5;
									for (int trunkHeight = 0; trunkHeight < treeHight; trunkHeight++)
									{
										SetVoxelComplex(FIndex(x, y, z + trunkHeight + 1), UVoxel::NewVoxel(EVoxelType::Oak, this));
									}
									for (int offsetZ = treeHight - leavesHight; offsetZ < treeHight + 1; offsetZ++)
									{
										for (int offsetX = -leavesWidth / 2; offsetX <= leavesWidth / 2; offsetX++)
										{
											for (int offsetY = -leavesWidth / 2; offsetY <= leavesWidth / 2; offsetY++)
											{
												UVoxel* voxel = GetVoxel(x + offsetX, y + offsetY, z + offsetZ + 1);
												if (!UVoxel::IsValid(voxel) || voxel->GetVoxelData().Transparency == ETransparency::Transparent)
												{
													SetVoxelComplex(FIndex(x + offsetX, y + offsetY, z + offsetZ + 1), UVoxel::NewVoxel(EVoxelType::Oak_Leaves, this));
												}
											}
										}
									}
								}
							}
							break;
						}
					}
					SetVoxelSample(voxelIndex, UVoxel::NewVoxel(voxelType, this));
				}
			}
		}
	}
}

void AChunk::LoadMap(FChunkData InChunkData)
{
	for (int32 i = 0; i < InChunkData.VoxelDatas.Num(); i++)
	{
		UVoxel* voxel = UVoxel::LoadVoxel(InChunkData.VoxelDatas[i], this);
		SetVoxelComplex(voxel->GetIndex(), voxel);
	}
}

void AChunk::LoadActors(FChunkData InChunkData)
{
	for (int32 i = 0; i < InChunkData.PickUpDatas.Num(); i++)
	{
		SpawnPickUp(InChunkData.PickUpDatas[i]);
	}
	for (int32 i = 0; i < InChunkData.CharacterDatas.Num(); i++)
	{
		SpawnCharacter(InChunkData.CharacterDatas[i]);
	}
	for (int32 i = 0; i < InChunkData.VitalityObjectDatas.Num(); i++)
	{
		SpawnVitalityObject(InChunkData.VitalityObjectDatas[i]);
	}
}

bool AChunk::GenerateMap()
{
	if (VoxelMap.Num() > 0)
	{
		for (auto iter = VoxelMap.CreateConstIterator(); iter; ++iter)
		{
			FIndex index = iter->Key;
			UVoxel* voxel = iter->Value;
			if (UVoxel::IsValid(voxel) && voxel->GetIndex() == index)
			{
				switch (voxel->GetVoxelData().Transparency)
				{
					case ETransparency::Solid:
					{
						if (GetSolidMesh())
						{
							GetSolidMesh()->BuildVoxel(voxel);
						}
						break;
					}
					case ETransparency::SemiTransparent:
					{
						if (GetSemiMesh())
						{
							GetSemiMesh()->BuildVoxel(voxel);
						}
						break;
					}
					case ETransparency::Transparent:
					{
						if (GetTransMesh())
						{
							GetTransMesh()->BuildVoxel(voxel);
						}
						break;
					}
				}
			}
		}
		return true;
	}
	return false;
}

void AChunk::GenerateMesh()
{
	if (GenerateMap())
	{
		if (SolidMesh)
		{
			SolidMesh->CreateMesh();
		}
		if (SemiMesh)
		{
			SemiMesh->CreateMesh();
		}
		if (TransMesh)
		{
			TransMesh->CreateMesh();
		}
	}

	if(!bGenerated)
	{
		OnGenerated();
	}
}

void AChunk::UpdateNeighbors(FIndex InIndex)
{
	UpdateNeighbors(InIndex.X, InIndex.Y, InIndex.Z);
}

void AChunk::UpdateNeighbors(int InX, int InY, int InZ)
{
	if (InX <= 0 && Neighbors[(int)EDirection::Back] != nullptr) {
		Neighbors[(int)EDirection::Back]->GenerateMesh();
	}
	else if (InX >= UDWHelper::GetWorldManager()->GetChunkSize() - 1 && Neighbors[(int)EDirection::Forward] != nullptr) {
		Neighbors[(int)EDirection::Forward]->GenerateMesh();
	}
	if (InY <= 0 && Neighbors[(int)EDirection::Left] != nullptr) {
		Neighbors[(int)EDirection::Left]->GenerateMesh();
	}
	else if (InY >= UDWHelper::GetWorldManager()->GetChunkSize() - 1 && Neighbors[(int)EDirection::Right] != nullptr) {
		Neighbors[(int)EDirection::Right]->GenerateMesh();
	}
	if (InZ <= 0 && Neighbors[(int)EDirection::Down] != nullptr) {
		Neighbors[(int)EDirection::Down]->GenerateMesh();
	}
	else if (InZ >= UDWHelper::GetWorldManager()->GetChunkSize() - 1 && Neighbors[(int)EDirection::Up] != nullptr) {
		Neighbors[(int)EDirection::Up]->GenerateMesh();
	}
}

bool AChunk::GenerateVoxel(FIndex InIndex, UVoxel* InVoxel)
{
	if(!InVoxel || !InVoxel->IsValidLowLevel()) return false;

	if(SetVoxelComplex(InIndex, InVoxel, true))
	{
		InVoxel->OnGenerate();
		if(InVoxel->GetVoxelData().GenerateSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, InVoxel->GetVoxelData().GenerateSound, IndexToLocation(InIndex));
		}
		return true;
	}
	else
	{
		InVoxel->OnDestroy();
		return false;
	}
}

bool AChunk::DestroyVoxel(FIndex InIndex)
{
	return DestroyVoxel(GetVoxel(InIndex));
}

bool AChunk::DestroyVoxel(UVoxel* InVoxel)
{
	if(!InVoxel || !InVoxel->IsValidLowLevel()) return false;

	if (SetVoxelComplex(InVoxel->GetIndex(), UVoxel::Empty(), true))
	{
		InVoxel->OnDestroy();
		if(InVoxel->GetVoxelData().GenerateSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, InVoxel->GetVoxelData().GenerateSound, IndexToLocation(InVoxel->GetIndex()));
		}
		return true;
	}
	return false;
}

bool AChunk::ReplaceVoxel(UVoxel* InOldVoxel, UVoxel* InNewVoxel)
{
	if(!InOldVoxel || !InOldVoxel->IsValidLowLevel() || !InNewVoxel || !InNewVoxel->IsValidLowLevel()) return false;

	if (SetVoxelComplex(InOldVoxel->GetIndex(), InNewVoxel, true))
	{
		InOldVoxel->OnReplace();
		InNewVoxel->OnGenerate();
		if(InNewVoxel->GetVoxelData().GenerateSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, InNewVoxel->GetVoxelData().GenerateSound, IndexToLocation(InNewVoxel->GetIndex()));
		}
		return true;
	}
	InNewVoxel->OnDestroy();
	return false;
}

APickUp* AChunk::SpawnPickUp(FItem InItem, FVector InLocation)
{
	if (InItem == FItem::Empty) return nullptr;

	APickUp* pickUpItem = nullptr;
	
	FActorSpawnParameters spawnParams = FActorSpawnParameters();
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	switch (InItem.GetData().Type)
	{
		case EItemType::Voxel:
		{
			pickUpItem = GetWorld()->SpawnActor<APickUpVoxel>(InLocation, FRotator::ZeroRotator, spawnParams);
			break;
		}
		case EItemType::Equip:
		{
			pickUpItem = GetWorld()->SpawnActor<APickUpEquip>(InLocation, FRotator::ZeroRotator, spawnParams);
			break;
		}
		case EItemType::Prop:
		{
			pickUpItem = GetWorld()->SpawnActor<APickUpProp>(InLocation, FRotator::ZeroRotator, spawnParams);
			break;
		}
	}
	
	if (pickUpItem != nullptr)
	{
		AttachPickUp(pickUpItem);
		pickUpItem->Initialize(InItem);
	}
	return pickUpItem;
}

APickUp* AChunk::SpawnPickUp(FPickUpData InPickUpData)
{
	return SpawnPickUp(InPickUpData.Item, InPickUpData.Location);
}

ADWCharacter* AChunk::SpawnCharacter(FCharacterData InSaveData)
{
	FActorSpawnParameters spawnParams = FActorSpawnParameters();
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	auto character = GetWorld()->SpawnActor<ADWCharacter>(InSaveData.Class, spawnParams);
	if (character != nullptr)
	{
		character->LoadData(InSaveData);
		AttachCharacter(character);
	}
	return character;
}

AVitalityObject* AChunk::SpawnVitalityObject(FVitalityObjectData InSaveData)
{
	FActorSpawnParameters spawnParams = FActorSpawnParameters();
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	auto vitalityObject = GetWorld()->SpawnActor<AVitalityObject>(InSaveData.Class, spawnParams);
	if (vitalityObject != nullptr)
	{
		vitalityObject->LoadData(InSaveData);
		AttachVitalityObject(vitalityObject);
	}
	return vitalityObject;
}

void AChunk::DestroyPickUp(APickUp* InPickUp)
{
	if(!InPickUp || !InPickUp->IsValidLowLevel()) return;

	if (PickUps.Contains(InPickUp))
	{
		PickUps.Remove(InPickUp);
		InPickUp->Destroy();
	}
}

void AChunk::DestroyCharacter(ADWCharacter* InCharacter)
{
	if (!InCharacter || !InCharacter->IsValidLowLevel()) return;

	if (Characters.Contains(InCharacter))
	{
		Characters.Remove(InCharacter);
		InCharacter->Destroy();
	}
}

void AChunk::DestroyVitalityObject(AVitalityObject* InVitalityObject)
{
	if (!InVitalityObject || !InVitalityObject->IsValidLowLevel()) return;

	if (VitalityObjects.Contains(InVitalityObject))
	{
		VitalityObjects.Remove(InVitalityObject);
		InVitalityObject->Destroy();
	}
}

void AChunk::AttachPickUp(APickUp* InPickUp)
{
	if(!InPickUp || !InPickUp->IsValidLowLevel() || PickUps.Contains(InPickUp)) return;

	InPickUp->OwnerChunk = this;
	InPickUp->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	PickUps.Add(InPickUp);
}

void AChunk::AttachCharacter(ADWCharacter* InCharacter)
{
	if(!InCharacter || !InCharacter->IsValidLowLevel() || Characters.Contains(InCharacter)) return;

	InCharacter->SetOwnerChunk(this);
	InCharacter->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	Characters.Add(InCharacter);
}

void AChunk::AttachVitalityObject(AVitalityObject* InVitalityObject)
{
	if(!InVitalityObject || !InVitalityObject->IsValidLowLevel() || VitalityObjects.Contains(InVitalityObject)) return;

	InVitalityObject->SetOwnerChunk(this);
	InVitalityObject->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	VitalityObjects.Add(InVitalityObject);
}

void AChunk::DetachPickUp(APickUp* InPickUp)
{
	if(!InPickUp || !InPickUp->IsValidLowLevel() || !PickUps.Contains(InPickUp)) return;

	InPickUp->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	PickUps.Remove(InPickUp);
}

void AChunk::DetachCharacter(ADWCharacter* InCharacter)
{
	if(!InCharacter || !InCharacter->IsValidLowLevel() || !Characters.Contains(InCharacter)) return;

	InCharacter->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	Characters.Remove(InCharacter);
}

void AChunk::DetachVitalityObject(AVitalityObject* InVitalityObject)
{
	if(!InVitalityObject || !InVitalityObject->IsValidLowLevel() || !VitalityObjects.Contains(InVitalityObject)) return;

	InVitalityObject->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	VitalityObjects.Remove(InVitalityObject);
}

bool AChunk::IsOnTheChunk(FIndex InIndex)
{
	return InIndex.X >= 0 && InIndex.X < UDWHelper::GetWorldManager()->GetChunkSize() &&
				InIndex.Y >= 0 && InIndex.Y < UDWHelper::GetWorldManager()->GetChunkSize() &&
				InIndex.Z >= 0 && InIndex.Z < UDWHelper::GetWorldManager()->GetChunkSize();
}

bool AChunk::IsOnTheChunk(FVector InLocation)
{
	return InLocation.X >= GetActorLocation().X && InLocation.X < GetActorLocation().X + UDWHelper::GetWorldManager()->GetChunkLength() &&
				InLocation.Y >= GetActorLocation().Y && InLocation.Y < GetActorLocation().Y + UDWHelper::GetWorldManager()->GetChunkLength() &&
				InLocation.Z >= GetActorLocation().Z && InLocation.Z < GetActorLocation().Z + UDWHelper::GetWorldManager()->GetChunkLength();
}

FIndex AChunk::LocationToIndex(FVector InLocation, bool bWorldSpace /*= true*/)
{
	FVector point = (!bWorldSpace ? InLocation : GetActorTransform().InverseTransformPosition(InLocation)) / UDWHelper::GetWorldManager()->GetBlockSize();

	FIndex index = FIndex::ZeroIndex;
	index.X = FMath::FloorToInt(point.X);
	index.Y = FMath::FloorToInt(point.Y);
	index.Z = FMath::FloorToInt(point.Z);

	return index;
}

FVector AChunk::IndexToLocation(FIndex InIndex, bool bWorldSpace /*= true*/)
{
	FVector localPoint = InIndex.ToVector() * UDWHelper::GetWorldManager()->GetBlockSize();
	if (!bWorldSpace) return localPoint;
	return GetActorTransform().TransformPosition(localPoint);
}

FIndex AChunk::LocalIndexToWorld(FIndex InIndex)
{
	return InIndex + Index * UDWHelper::GetWorldManager()->GetChunkSize();
}

FIndex AChunk::WorldIndexToLocal(FIndex InIndex)
{
	return InIndex - Index * UDWHelper::GetWorldManager()->GetChunkSize();
}

UVoxelMeshComponent* AChunk::GetSolidMesh()
{
	if (!SolidMesh)
	{
		SolidMesh = NewObject<UVoxelMeshComponent>(this, TEXT("SolidMesh"));
		SolidMesh->RegisterComponent();
		SolidMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		SolidMesh->Initialize(EVoxelMeshType::Chunk, ETransparency::Solid);
		SolidMesh->SetCollisionProfileName(TEXT("DW_SolidVoxel"));
	}
	return SolidMesh;
}

UVoxelMeshComponent* AChunk::GetSemiMesh()
{
	if (!SemiMesh)
	{
		SemiMesh = NewObject<UVoxelMeshComponent>(this, TEXT("SemiMesh"));
		SemiMesh->RegisterComponent();
		SemiMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		SemiMesh->Initialize(EVoxelMeshType::Chunk, ETransparency::SemiTransparent);
		SemiMesh->SetCollisionProfileName(TEXT("DW_SolidVoxel"));
	}
	return SemiMesh;
}

UVoxelMeshComponent* AChunk::GetTransMesh()
{
	if (!TransMesh)
	{
		TransMesh = NewObject<UVoxelMeshComponent>(this, TEXT("TransMesh"));
		TransMesh->RegisterComponent();
		TransMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		TransMesh->Initialize(EVoxelMeshType::Chunk, ETransparency::Transparent);
		TransMesh->SetCollisionProfileName(TEXT("DW_TransVoxel"));
	}
	return TransMesh;
}