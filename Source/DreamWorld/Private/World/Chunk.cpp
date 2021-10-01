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
#include "ObjectPoolModuleBPLibrary.h"
#include "DataSave/WorldDataSave.h"
#include "Engine/World.h"
#include "PickUp/PickUpVoxel.h"
#include "Character/DWCharacter.h"
#include "DataSave/GameDataSave.h"
#include "Engine/World.h"
#include "PickUp/PickUpEquip.h"
#include "PickUp/PickUpProp.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
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
	VoxelMap = TMap<FIndex, FVoxelItem>();
	Neighbors = TArray<AChunk*>();
	Neighbors.SetNumZeroed(6);
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
	if(!OtherActor) return;
		
	if(OtherActor->IsA(ADWCharacter::StaticClass()))
	{
		if(ADWCharacter* character = Cast<ADWCharacter>(OtherActor))
		{
			UVoxel* voxel = GetVoxel(LocationToIndex(Hit.ImpactPoint - AWorldManager::GetWorldData().GetBlockSizedNormal(Hit.ImpactNormal, 0.01f)));
			if (UVoxel::IsValid(voxel))
			{
				voxel->OnTargetHit(character, FVoxelHitResult(voxel, Hit.ImpactPoint, Hit.ImpactNormal));
			}
			UVoxel::DespawnVoxel(this, voxel);
		}
	}
}

void AChunk::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!OtherActor) return;
		
	if(OtherActor->IsA(ADWCharacter::StaticClass()))
	{
		if(ADWCharacter* character = Cast<ADWCharacter>(OtherActor))
		{
			UVoxel* voxel = GetVoxel(LocationToIndex(SweepResult.ImpactPoint + AWorldManager::GetWorldData().GetBlockSizedNormal(SweepResult.ImpactNormal)));
			if (UVoxel::IsValid(voxel))
			{
				if (character->GetOverlapVoxel())
				{
					if(character->GetOverlapVoxel()->GetVoxelData().VoxelType != EVoxelType::Water)
					{
						character->GetOverlapVoxel()->OnTargetExit(character, FVoxelHitResult(voxel, SweepResult.ImpactPoint, SweepResult.ImpactNormal));
					}
					else if(!UVoxel::IsValid(GetVoxel(LocationToIndex(character->GetActorLocation()))))
					{
						character->GetOverlapVoxel()->OnTargetExit(character, FVoxelHitResult(voxel, SweepResult.ImpactPoint, SweepResult.ImpactNormal));
					}
				}
				voxel->OnTargetEnter(character, FVoxelHitResult(voxel, SweepResult.ImpactPoint, SweepResult.ImpactNormal));
			}
			UVoxel::DespawnVoxel(this, voxel);
		}
	}
}

void AChunk::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(!OtherActor) return;
		
	if (OtherActor->IsA(ADWCharacter::StaticClass()))
	{
		if(ADWCharacter* character = Cast<ADWCharacter>(OtherActor))
		{
			UVoxel* voxel = GetVoxel(LocationToIndex(character->GetActorLocation()));
			if (UVoxel::IsValid(voxel))
			{
				voxel->OnTargetEnter(character, FVoxelHitResult(voxel, character->GetActorLocation(), character->GetMoveDirection()));
			}
			else if(character->GetOverlapVoxel())
			{
				character->GetOverlapVoxel()->OnTargetExit(character, FVoxelHitResult(voxel, character->GetActorLocation(), character->GetMoveDirection()));
			}
			UVoxel::DespawnVoxel(this, voxel);
		}
	}
}

// Called every frame
void AChunk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AChunk::OnSpawn_Implementation()
{
}

void AChunk::OnDespawn_Implementation()
{
	if (bGenerated)
	{
		FChunkData chunkData;

		chunkData.Index = Index;
		chunkData.bSaved = true;

		for (auto& iter : VoxelMap)
		{
			chunkData.VoxelItems.Add(iter.Value);
			iter.Value.Owner = nullptr;
			if(iter.Value.Auxiliary)
			{
				iter.Value.Auxiliary->Destroy();
			}
			iter.Value.Auxiliary = nullptr;
		}
		for (int32 i = 0; i < PickUps.Num(); i++)
		{
			chunkData.PickUpDatas.Add(PickUps[i]->ToData());
			PickUps[i]->Destroy();
		}
		for (int32 i = 0; i < Characters.Num(); i++)
		{
			if(Characters[i]->GetNature() != ECharacterNature::Player)
			{
				chunkData.CharacterDatas.Add(Characters[i]->ToData());
				Characters[i]->Destroy();
			}
		}
		for (int32 i = 0; i < VitalityObjects.Num(); i++)
		{
			chunkData.VitalityObjectDatas.Add(VitalityObjects[i]->ToData());
			VitalityObjects[i]->Destroy();
		}
		if(UWorldDataSave* WorldDataSave = AWorldManager::GetDataSave())
		{
			WorldDataSave->SaveChunkData(Index, chunkData);
		}
	}

	BreakNeighbors();

	Index = FIndex::ZeroIndex;
	Batch = 0;
	bGenerated = false;
	
	VoxelMap.Empty();
	PickUps.Empty();
	Characters.Empty();
	VitalityObjects.Empty();

	DestroySolidMesh();
	DestroySemiMesh();
	DestroyTransMesh();
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
		SolidMesh->OnComponentHit.AddDynamic(this, &AChunk::OnCollision);
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
		SolidMesh->OnComponentHit.AddDynamic(this, &AChunk::OnCollision);
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
		SolidMesh->OnComponentBeginOverlap.AddDynamic(this, &AChunk::OnBeginOverlap);
		SolidMesh->OnComponentEndOverlap.AddDynamic(this, &AChunk::OnEndOverlap);
	}
	return TransMesh;
}

void AChunk::DestroySolidMesh()
{
	if(SolidMesh)
	{
		SolidMesh->DestroyComponent();
		SolidMesh = nullptr;
	}
}

void AChunk::DestroySemiMesh()
{
	if(SemiMesh)
	{
		SemiMesh->DestroyComponent();
		SemiMesh = nullptr;
	}
}

void AChunk::DestroyTransMesh()
{
	if(TransMesh)
	{
		TransMesh->DestroyComponent();
		TransMesh = nullptr;
	}
}

void AChunk::Initialize(FIndex InIndex, int32 InBatch)
{
	Index = InIndex;
	Batch = InBatch;
	UpdateNeighbors();
}

void AChunk::Generate()
{
	if (VoxelMap.Num() > 0)
	{
		if(bGenerated)
		{
			GenerateMap();
		}
		if (SolidMesh)
		{
			if(!SolidMesh->IsEmpty())
			{
				SolidMesh->CreateMesh();
			}
			else
			{
				DestroySolidMesh();
			}
		}
		if (SemiMesh)
		{
			if(!SemiMesh->IsEmpty())
			{
				SemiMesh->CreateMesh();
			}
			else
			{
				DestroySemiMesh();
			}
		}
		if (TransMesh)
		{
			if(!TransMesh->IsEmpty())
			{
				TransMesh->CreateMesh();
			}
			else
			{
				DestroyTransMesh();
			}
		}
	}
	else
	{
		DestroySolidMesh();
		DestroySemiMesh();
		DestroyTransMesh();
	}

	if(!bGenerated)
	{
		OnGenerated();
	}
}

void AChunk::GenerateMap()
{
	for (auto& iter : VoxelMap)
	{
		FVoxelItem& voxelItem = iter.Value;
		if (voxelItem.IsValid())
		{
			switch (voxelItem.GetVoxelData().Transparency)
			{
				case ETransparency::Solid:
				{
					if (GetSolidMesh())
					{
						GetSolidMesh()->BuildVoxel(voxelItem);
					}
					break;
				}
				case ETransparency::SemiTransparent:
				{
					if (GetSemiMesh())
					{
						GetSemiMesh()->BuildVoxel(voxelItem);
					}
					break;
				}
				case ETransparency::Transparent:
				{
					if (GetTransMesh())
					{
						GetTransMesh()->BuildVoxel(voxelItem);
					}
					break;
				}
			}
		}
	}
}

void AChunk::BuildMap()
{
	for (int x = 0; x < AWorldManager::GetWorldData().ChunkSize; x++)
	{
		for (int y = 0; y < AWorldManager::GetWorldData().ChunkSize; y++)
		{
			for (int z = 0; z < AWorldManager::GetWorldData().ChunkSize; z++)
			{
				FIndex voxelIndex = FIndex(x, y, z);

				if (VoxelMap.Contains(voxelIndex)) continue;

				const EVoxelType voxelType = AWorldManager::GetNoiseVoxelType(LocalIndexToWorld(voxelIndex));

				if (voxelType != EVoxelType::Empty)
				{
					switch (voxelType)
					{
						// grass
						case EVoxelType::Grass:
						{
							const float tmpNum = FMath::FRandRange(0, 1);
							// plant
							if (tmpNum < 0.2f)
							{
								SetVoxelComplex(FIndex(x, y, z + 1), UVoxel::SpawnVoxel(this, FMath::FRandRange(0, 1) > 0.2f ? EVoxelType::Tall_Grass : (EVoxelType)FMath::RandRange((int)EVoxelType::Flower_Allium, (int32)EVoxelType::Flower_Tulip_White)));
							}
							// tree
							else if (tmpNum < 0.21f)
							{
								if ((x > 2 && x <= AWorldManager::GetWorldData().ChunkSize - 2) && (y > 2 && y <= AWorldManager::GetWorldData().ChunkSize - 2))
								{
									const int treeHeight = FMath::RandRange(4, 5);
									const int leavesHeight = 2/*FMath::RandRange(2, 2)*/;
									const int leavesWidth = FMath::FRandRange(0, 1) < 0.5f ? 3 : 5;
									for (int trunkHeight = 0; trunkHeight < treeHeight; trunkHeight++)
									{
										SetVoxelComplex(FIndex(x, y, z + trunkHeight + 1), UVoxel::SpawnVoxel(this, EVoxelType::Oak));
									}
									for (int offsetZ = treeHeight - leavesHeight; offsetZ < treeHeight + 1; offsetZ++)
									{
										for (int offsetX = -leavesWidth / 2; offsetX <= leavesWidth / 2; offsetX++)
										{
											for (int offsetY = -leavesWidth / 2; offsetY <= leavesWidth / 2; offsetY++)
											{
												UVoxel* voxel = GetVoxel(x + offsetX, y + offsetY, z + offsetZ + 1);
												if (!UVoxel::IsValid(voxel) || voxel->GetVoxelData().Transparency == ETransparency::Transparent)
												{
													SetVoxelComplex(FIndex(x + offsetX, y + offsetY, z + offsetZ + 1), UVoxel::SpawnVoxel(this, EVoxelType::Oak_Leaves));
												}
											}
										}
									}
								}
							}
							break;
						}
						default: break;
					}
					SetVoxelSample(voxelIndex, UVoxel::SpawnVoxel(this, voxelType));
				}
			}
		}
	}
}

void AChunk::LoadMap(FChunkData InChunkData)
{
	for (int32 i = 0; i < InChunkData.VoxelItems.Num(); i++)
	{
		UVoxel* tmpVoxel = UVoxel::LoadVoxel(this, InChunkData.VoxelItems[i]);
		SetVoxelComplex(tmpVoxel->GetIndex(), tmpVoxel);
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

void AChunk::OnGenerated()
{
	bGenerated = true;
	
	for (auto& iter : VoxelMap)
	{
		SpawnAuxiliary(iter.Value);
	}
	
	if (UWorldDataSave* WorldDataSave = AWorldManager::GetDataSave())
	{
		if (WorldDataSave->IsExistChunkData(Index))
		{
			LoadActors(WorldDataSave->LoadChunkData(Index));
		}
		else if(VoxelMap.Num() > 0)
		{
			if (FMath::FRand() <= AWorldManager::GetWorldData().VitalityRateDensity)
			{
				auto raceData = UDWHelper::RandomVitalityRaceData();
				auto tmpNum = FMath::RandRange(raceData.MinNum, raceData.MaxNum);
				for (int32 j = 0; j < tmpNum; j++)
				{
					FHitResult hitResult;
					auto raceItem = raceData.RaceItems[FMath::RandRange(0, raceData.RaceItems.Num() - 1)];
					if (AWorldManager::GetCurrent()->ChunkTraceSingle(this, FMath::Max(raceItem.Range.X, raceItem.Range.Y) * 0.5f * AWorldManager::GetWorldData().BlockSize, raceItem.Range.Z * 0.5f * AWorldManager::GetWorldData().BlockSize, hitResult))
					{
						auto saveData = FVitalityObjectData();
						saveData.SpawnClass = raceItem.Class;
						saveData.RaceID = raceData.ID.ToString();
						saveData.SpawnLocation = hitResult.Location;
						saveData.SpawnRotation = FRotator(0, FMath::RandRange(0, 360), 0);
						SpawnVitalityObject(saveData);
					}
				}
			}
			if (FMath::FRand() <= AWorldManager::GetWorldData().CharacterRateDensity)
			{
				ADWCharacter* captain = nullptr;
				auto raceData = UDWHelper::RandomCharacterRaceData();
				auto tmpNum = FMath::RandRange(raceData.MinNum, raceData.MaxNum);
				for (int32 j = 0; j < tmpNum; j++)
				{
					FHitResult hitResult;
					auto raceItem = raceData.RaceItems[FMath::RandRange(0, raceData.RaceItems.Num() - 1)];
					if (AWorldManager::GetCurrent()->ChunkTraceSingle(this, FMath::Max(raceItem.Range.X, raceItem.Range.Y) * 0.5f * AWorldManager::GetWorldData().BlockSize, raceItem.Range.Z * 0.5f * AWorldManager::GetWorldData().BlockSize, hitResult))
					{
						auto saveData = FCharacterData();
						saveData.RaceID = raceData.ID.ToString();
						saveData.SpawnClass = raceItem.Class;
						saveData.SpawnLocation = hitResult.Location;
						saveData.SpawnRotation = FRotator(0, FMath::RandRange(0, 360), 0);

						auto character = SpawnCharacter(saveData);

						if (character != nullptr)
						{
							if (captain == nullptr)
							{
								captain = character;
								AWorldManager::GetCurrent()->CreateTeam(captain);
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
}

void AChunk::GenerateNeighbors(FIndex InIndex)
{
	GenerateNeighbors(InIndex.X, InIndex.Y, InIndex.Z);
}

void AChunk::GenerateNeighbors(int InX, int InY, int InZ)
{
	if (InX <= 0 && Neighbors[(int32)EDirection::Back] != nullptr) {
		Neighbors[(int32)EDirection::Back]->Generate();
	}
	else if (InX >= AWorldManager::GetWorldData().ChunkSize - 1 && Neighbors[(int32)EDirection::Forward] != nullptr) {
		Neighbors[(int32)EDirection::Forward]->Generate();
	}
	if (InY <= 0 && Neighbors[(int32)EDirection::Left] != nullptr) {
		Neighbors[(int32)EDirection::Left]->Generate();
	}
	else if (InY >= AWorldManager::GetWorldData().ChunkSize - 1 && Neighbors[(int32)EDirection::Right] != nullptr) {
		Neighbors[(int32)EDirection::Right]->Generate();
	}
	if (InZ <= 0 && Neighbors[(int32)EDirection::Down] != nullptr) {
		Neighbors[(int32)EDirection::Down]->Generate();
	}
	else if (InZ >= AWorldManager::GetWorldData().ChunkSize - 1 && Neighbors[(int32)EDirection::Up] != nullptr) {
		Neighbors[(int32)EDirection::Up]->Generate();
	}
}

void AChunk::UpdateNeighbors()
{
	for (int i = 0; i < 6; i++)
	{
		Neighbors[i] = AWorldManager::GetCurrent()->FindChunk(Index + UDWHelper::DirectionToIndex((EDirection)i));
		if (Neighbors[i] != nullptr)
		{
			Neighbors[i]->Neighbors[(int32)UDWHelper::InvertDirection((EDirection)i)] = this;
		}
	}
}

void AChunk::BreakNeighbors()
{
	for (int i = 0; i < 6; i++)
	{
		if (Neighbors[i] != nullptr)
		{
			Neighbors[i]->Neighbors[(int32)UDWHelper::InvertDirection((EDirection)i)] = nullptr;
			Neighbors[i] = nullptr;
		}
	}
}

UVoxel* AChunk::GetVoxel(FIndex InIndex)
{
	return GetVoxel(InIndex.X, InIndex.Y, InIndex.Z);
}

UVoxel* AChunk::GetVoxel(int InX, int InY, int InZ)
{
	if (InX < 0) {
		if (Neighbors[(int32)EDirection::Back] != nullptr)
			return Neighbors[(int32)EDirection::Back]->GetVoxel(InX + AWorldManager::GetWorldData().ChunkSize, InY, InZ);
		return UVoxel::UnknownVoxel;
	}
	else if (InX >= AWorldManager::GetWorldData().ChunkSize) {
		if (Neighbors[(int32)EDirection::Forward] != nullptr)
			return Neighbors[(int32)EDirection::Forward]->GetVoxel(InX - AWorldManager::GetWorldData().ChunkSize, InY, InZ);
		return UVoxel::UnknownVoxel;
	}
	else if (InY < 0) {
		if (Neighbors[(int32)EDirection::Left] != nullptr)
			return Neighbors[(int32)EDirection::Left]->GetVoxel(InX, InY + AWorldManager::GetWorldData().ChunkSize, InZ);
		return UVoxel::UnknownVoxel;
	}
	else if (InY >= AWorldManager::GetWorldData().ChunkSize) {
		if (Neighbors[(int32)EDirection::Right] != nullptr)
			return Neighbors[(int32)EDirection::Right]->GetVoxel(InX, InY - AWorldManager::GetWorldData().ChunkSize, InZ);
		return UVoxel::UnknownVoxel;
	}
	else if (InZ < 0) {
		if (Neighbors[(int32)EDirection::Down] != nullptr)
			return Neighbors[(int32)EDirection::Down]->GetVoxel(InX, InY, InZ + AWorldManager::GetWorldData().ChunkSize);
		else if (Index.Z > 0)
			return UVoxel::UnknownVoxel;
	}
	else if (InZ >= AWorldManager::GetWorldData().ChunkSize) {
		if (Neighbors[(int32)EDirection::Up] != nullptr)
			return Neighbors[(int32)EDirection::Up]->GetVoxel(InX, InY, InZ - AWorldManager::GetWorldData().ChunkSize);
		else if (Index.Z < AWorldManager::GetWorldData().ChunkHeightRange)
			return UVoxel::UnknownVoxel;
	}
	else if (VoxelMap.Contains(FIndex(InX, InY, InZ))) {
		return VoxelMap[FIndex(InX, InY, InZ)].GetVoxel();
	}
	return UVoxel::EmptyVoxel;
}

FVoxelItem& AChunk::GetVoxelItem(FIndex InIndex)
{
	return GetVoxelItem(InIndex.X, InIndex.Y, InIndex.Z);
}

FVoxelItem& AChunk::GetVoxelItem(int InX, int InY, int InZ)
{
	if (InX < 0) {
		if (Neighbors[(int32)EDirection::Back] != nullptr)
			return Neighbors[(int32)EDirection::Back]->GetVoxelItem(InX + AWorldManager::GetWorldData().ChunkSize, InY, InZ);
		return FVoxelItem::UnknownVoxel;
	}
	else if (InX >= AWorldManager::GetWorldData().ChunkSize) {
		if (Neighbors[(int32)EDirection::Forward] != nullptr)
			return Neighbors[(int32)EDirection::Forward]->GetVoxelItem(InX - AWorldManager::GetWorldData().ChunkSize, InY, InZ);
		return FVoxelItem::UnknownVoxel;
	}
	else if (InY < 0) {
		if (Neighbors[(int32)EDirection::Left] != nullptr)
			return Neighbors[(int32)EDirection::Left]->GetVoxelItem(InX, InY + AWorldManager::GetWorldData().ChunkSize, InZ);
		return FVoxelItem::UnknownVoxel;
	}
	else if (InY >= AWorldManager::GetWorldData().ChunkSize) {
		if (Neighbors[(int32)EDirection::Right] != nullptr)
			return Neighbors[(int32)EDirection::Right]->GetVoxelItem(InX, InY - AWorldManager::GetWorldData().ChunkSize, InZ);
		return FVoxelItem::UnknownVoxel;
	}
	else if (InZ < 0) {
		if (Neighbors[(int32)EDirection::Down] != nullptr)
			return Neighbors[(int32)EDirection::Down]->GetVoxelItem(InX, InY, InZ + AWorldManager::GetWorldData().ChunkSize);
		else if (Index.Z > 0)
			return FVoxelItem::UnknownVoxel;
	}
	else if (InZ >= AWorldManager::GetWorldData().ChunkSize) {
		if (Neighbors[(int32)EDirection::Up] != nullptr)
			return Neighbors[(int32)EDirection::Up]->GetVoxelItem(InX, InY, InZ - AWorldManager::GetWorldData().ChunkSize);
		else if (Index.Z < AWorldManager::GetWorldData().ChunkHeightRange)
			return FVoxelItem::UnknownVoxel;
	}
	else if (VoxelMap.Contains(FIndex(InX, InY, InZ))) {
		return VoxelMap[FIndex(InX, InY, InZ)];
	}
	return FVoxelItem::EmptyVoxel;
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
						UVoxel::DespawnVoxel(Owner, voxel);
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
			UVoxel::DespawnVoxel(Owner, voxel);
			return true;
		}
	}
	return false;
}

bool AChunk::CheckVoxel(int InX, int InY, int InZ, FVector InRange/* = FVector::OneVector*/, bool bIgnoreTransparent /*= true*/)
{
	return CheckVoxel(FIndex(InX, InY, InZ), InRange, bIgnoreTransparent);
}

bool AChunk::CheckAdjacent(FIndex InIndex, EDirection InDirection)
{
	if(InDirection == EDirection::Down && LocalIndexToWorld(InIndex).Z == 0) return false;

	FVoxelItem& voxelItem = GetVoxelItem(InIndex);
	FVoxelData voxelData = voxelItem.GetVoxelData();
	
	FVoxelItem& adjacentItem = GetVoxelItem(UDWHelper::GetAdjacentIndex(InIndex, InDirection, voxelItem.Rotation));
	FVoxelData adjacentData = adjacentItem.GetVoxelData();

	if (adjacentItem.IsValid())
	{
		if(voxelData.Range * voxelItem.Scale == adjacentData.Range * adjacentItem.Scale)
		{
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
						default: break;
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
									default: break;
								}
							}
							break;
						}
						default: break;
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
									default: break;
								}
							}
							break;
						}
					}
					break;
				}
			}
		}
	}
	else if(adjacentItem == FVoxelItem::UnknownVoxel)
	{
		return false;
	}
	return true;
}

bool AChunk::CheckNeighbors(FIndex InIndex, EVoxelType InVoxelType, bool bIgnoreBottom, int InDistance)
{
	FVoxelItem& voxelItem = GetVoxelItem(InIndex);
	const FVector range = voxelItem.GetVoxelData().GetCeilRange(voxelItem.Rotation, voxelItem.Scale);
	for (int x = -InDistance; x < range.X + InDistance; x++)
	{
		for (int y = -InDistance; y < range.Y + InDistance; y++)
		{
			for (int z = bIgnoreBottom ? 0 : -InDistance; z < range.Z + InDistance; z++)
			{
				FVoxelItem& neighborItem = GetVoxelItem(Index + FIndex(x, y, z));
				if (neighborItem.IsValid())
				{
					if (neighborItem.GetVoxelData().VoxelType == InVoxelType)
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool AChunk::SetVoxelSample(FIndex InIndex, UVoxel* InVoxel, bool bGenerateMesh, bool bAutoDespawn)
{
	bool RetValue = false;

	if (UVoxel::IsValid(InVoxel, false))
	{
		if (!CheckVoxel(InIndex))
		{
			InVoxel->Initialize(InIndex, this);
			if (!VoxelMap.Contains(InIndex))
			{
				VoxelMap.Add(InIndex, InVoxel->ToItem());
			}
			else
			{
				UVoxel* voxel = GetVoxel(InIndex);
				if (bGenerateMesh && UVoxel::IsValid(voxel))
				{
					voxel->OnDestroy();
				}
				UVoxel::DespawnVoxel(this, voxel);
				VoxelMap[InIndex] = InVoxel->ToItem();
			}
			RetValue = true;
		}
	}
	else if (VoxelMap.Contains(InIndex))
	{
		VoxelMap.Remove(InIndex);
		RetValue = true;
	}

	if (RetValue && bGenerateMesh)
	{
		Generate();
		GenerateNeighbors(InIndex);
	}

	if(bAutoDespawn)
	{
		UVoxel::DespawnVoxel(this, InVoxel);
	}

	return RetValue;
}

bool AChunk::SetVoxelSample(int InX, int InY, int InZ, UVoxel* InVoxel, bool bGenerateMesh, bool bAutoDespawn)
{
	return SetVoxelSample(FIndex(InX, InY, InZ), InVoxel, bGenerateMesh, bAutoDespawn);
}

bool AChunk::SetVoxelComplex(FIndex InIndex, UVoxel* InVoxel, bool bGenerateMesh, bool bAutoDespawn)
{
	return SetVoxelComplex(InIndex.X, InIndex.Y, InIndex.Z, InVoxel, bGenerateMesh, bAutoDespawn);
}

bool AChunk::SetVoxelComplex(int InX, int InY, int InZ, UVoxel* InVoxel, bool bGenerateMesh, bool bAutoDespawn)
{
	if (InX < 0) {
		if (Neighbors[(int32)EDirection::Back] != nullptr)
			return Neighbors[(int32)EDirection::Back]->SetVoxelComplex(InX + AWorldManager::GetWorldData().ChunkSize, InY, InZ, InVoxel, bGenerateMesh);
	}
	else if (InX >= AWorldManager::GetWorldData().ChunkSize) {
		if (Neighbors[(int32)EDirection::Forward] != nullptr)
			return Neighbors[(int32)EDirection::Forward]->SetVoxelComplex(InX - AWorldManager::GetWorldData().ChunkSize, InY, InZ, InVoxel, bGenerateMesh);
	}
	else if (InY < 0) {
		if (Neighbors[(int32)EDirection::Left] != nullptr)
			return Neighbors[(int32)EDirection::Left]->SetVoxelComplex(InX, InY + AWorldManager::GetWorldData().ChunkSize, InZ, InVoxel, bGenerateMesh);
	}
	else if (InY >= AWorldManager::GetWorldData().ChunkSize) {
		if (Neighbors[(int32)EDirection::Right] != nullptr)
			return Neighbors[(int32)EDirection::Right]->SetVoxelComplex(InX, InY - AWorldManager::GetWorldData().ChunkSize, InZ, InVoxel, bGenerateMesh);
	}
	else if (InZ < 0) {
		if (Neighbors[(int32)EDirection::Down] != nullptr)
			return Neighbors[(int32)EDirection::Down]->SetVoxelComplex(InX, InY, InZ + AWorldManager::GetWorldData().ChunkSize, InVoxel, bGenerateMesh);
	}
	else if (InZ >= AWorldManager::GetWorldData().ChunkSize) {
		if (Neighbors[(int32)EDirection::Up] == nullptr)
			AWorldManager::GetCurrent()->SpawnChunk(Index + UDWHelper::DirectionToIndex(EDirection::Up), !bGenerateMesh);
		if (Neighbors[(int32)EDirection::Up] != nullptr)
			return Neighbors[(int32)EDirection::Up]->SetVoxelComplex(InX, InY, InZ - AWorldManager::GetWorldData().ChunkSize, InVoxel, bGenerateMesh);
	}
	else {
		if (InVoxel->GetVoxelData().IsComplex())
		{
			if (InVoxel != UVoxel::EmptyVoxel)
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
					UVoxel::DespawnVoxel(this, voxel);
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

bool AChunk::GenerateVoxel(FIndex InIndex, UVoxel* InVoxel)
{
	if(!InVoxel || !InVoxel->IsValidLowLevel()) return false;

	if(SetVoxelComplex(InIndex, InVoxel, true, false))
	{
		SpawnAuxiliary(GetVoxelItem(InIndex));
		if(InVoxel->GetVoxelData().GenerateSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, InVoxel->GetVoxelData().GenerateSound, IndexToLocation(InIndex));
		}
		return true;
	}
	UVoxel::DespawnVoxel(this, InVoxel);
	return false;
}

bool AChunk::DestroyVoxel(FIndex InIndex)
{
	return DestroyVoxel(GetVoxel(InIndex));
}

bool AChunk::DestroyVoxel(UVoxel* InVoxel)
{
	if(!InVoxel || !InVoxel->IsValidLowLevel()) return false;

	if (SetVoxelComplex(InVoxel->GetIndex(), UVoxel::EmptyVoxel, true, false))
	{
		if(InVoxel->GetVoxelData().GenerateSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, InVoxel->GetVoxelData().GenerateSound, IndexToLocation(InVoxel->GetIndex()));
		}
		// if (Owner && Owner->IsValidLowLevel())
		// {
		// 	FVector range = GetVoxelData().GetCeilRange(this);
		// 	for (int x = 0; x < range.X; x++)
		// 	{
		// 		for (int y = 0; y < range.Y; y++)
		// 		{
		// 			FIndex index = Index + FIndex(x, y, range.Z);
		// 			UVoxel* voxel = Owner->GetVoxel(index);
		// 			if (UVoxel::IsValid(voxel))
		// 			{
		// 				FVoxelData voxelData = voxel->GetVoxelData();
		// 				if(voxelData.Transparency == ETransparency::Transparent && voxelData.VoxelType != EVoxelType::Water)
		// 				{
		// 					Owner->DestroyVoxel(index);
		// 				}
		// 			}
		// 			UVoxel::DespawnVoxel(Owner, voxel);
		// 		}
		// 	}
		// 	if (Owner->CheckNeighbors(Index, EVoxelType::Water, true))
		// 	{
		// 		for (int x = 0; x < range.X; x++)
		// 		{
		// 			for (int y = 0; y < range.Y; y++)
		// 			{
		// 				for (int z = 0; z < range.Z; z++)
		// 				{
		// 					Owner->SetVoxelComplex(Index + FIndex(x, y, z), UVoxel::SpawnVoxel(this, EVoxelType::Water), true);
		// 				}
		// 			}
		// 		}
		// 	}
		// 	if(GetVoxelData().GenerateSound) UGameplayStatics::PlaySoundAtLocation(this, GetVoxelData().GenerateSound, Owner->IndexToLocation(Index));
		// 	Owner->SpawnPickUp(FItem(ID, 1), Owner->IndexToLocation(Index) + range * AWorldManager::GetWorldData().BlockSize * 0.5f);
		//
		// 	Owner->DestroyAuxiliary(Auxiliary);
		// }
		UVoxel::DespawnVoxel(this, InVoxel);
		return true;
	}
	return false;
}

bool AChunk::ReplaceVoxel(UVoxel* InOldVoxel, UVoxel* InNewVoxel)
{
	if(!InOldVoxel || !InOldVoxel->IsValidLowLevel() || !InNewVoxel || !InNewVoxel->IsValidLowLevel()) return false;

	if (SetVoxelComplex(InOldVoxel->GetIndex(), InNewVoxel, true, false))
	{
		SpawnAuxiliary(GetVoxelItem(InNewVoxel->GetIndex()));
		if(InNewVoxel->GetVoxelData().GenerateSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, InNewVoxel->GetVoxelData().GenerateSound, IndexToLocation(InNewVoxel->GetIndex()));
		}
		UVoxel::DespawnVoxel(this, InNewVoxel);
		return true;
	}
	UVoxel::DespawnVoxel(this, InNewVoxel);
	return false;
}

bool AChunk::IsOnTheChunk(FIndex InIndex) const
{
	return InIndex.X >= 0 && InIndex.X < AWorldManager::GetWorldData().ChunkSize &&
		InIndex.Y >= 0 && InIndex.Y < AWorldManager::GetWorldData().ChunkSize &&
		InIndex.Z >= 0 && InIndex.Z < AWorldManager::GetWorldData().ChunkSize;
}

bool AChunk::IsOnTheChunk(FVector InLocation) const
{
	return InLocation.X >= GetActorLocation().X && InLocation.X < GetActorLocation().X + AWorldManager::GetWorldData().GetChunkLength() &&
			InLocation.Y >= GetActorLocation().Y && InLocation.Y < GetActorLocation().Y + AWorldManager::GetWorldData().GetChunkLength() &&
			InLocation.Z >= GetActorLocation().Z && InLocation.Z < GetActorLocation().Z + AWorldManager::GetWorldData().GetChunkLength();
}

FIndex AChunk::LocationToIndex(FVector InLocation, bool bWorldSpace /*= true*/) const
{
	FVector point = (!bWorldSpace ? InLocation : GetActorTransform().InverseTransformPosition(InLocation)) / AWorldManager::GetWorldData().BlockSize;

	FIndex index;
	index.X = FMath::FloorToInt(point.X);
	index.Y = FMath::FloorToInt(point.Y);
	index.Z = FMath::FloorToInt(point.Z);

	return index;
}

FVector AChunk::IndexToLocation(FIndex InIndex, bool bWorldSpace /*= true*/) const
{
	const FVector localPoint = InIndex.ToVector() * AWorldManager::GetWorldData().BlockSize;
	if (!bWorldSpace) return localPoint;
	return GetActorTransform().TransformPosition(localPoint);
}

FIndex AChunk::LocalIndexToWorld(FIndex InIndex) const
{
	return InIndex + Index * AWorldManager::GetWorldData().ChunkSize;
}

FIndex AChunk::WorldIndexToLocal(FIndex InIndex) const
{
	return InIndex - Index * AWorldManager::GetWorldData().ChunkSize;
}

AVoxelAuxiliary* AChunk::SpawnAuxiliary(FVoxelItem& InVoxelItem)
{
	const FVoxelData VoxelData = InVoxelItem.GetVoxelData();
	if (VoxelMap.Contains(InVoxelItem.Index) && VoxelData.AuxiliaryClass)
	{
		FActorSpawnParameters spawnParams = FActorSpawnParameters();
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		const FVector Location = IndexToLocation(InVoxelItem.Index, true) + VoxelData.GetCeilRange() * AWorldManager::GetWorldData().BlockSize * 0.5f;
		if (AVoxelAuxiliary* Auxiliary = GetWorld()->SpawnActor<AVoxelAuxiliary>(VoxelData.AuxiliaryClass, Location, InVoxelItem.Rotation, spawnParams))
		{
			InVoxelItem.Auxiliary = Auxiliary;
			Auxiliary->Initialize(this, InVoxelItem.Index);
			Auxiliary->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
			//SetVoxelSample(InVoxelItem.Index, InVoxelItem, false, false);
			return Auxiliary;
		}
	}
	return nullptr;
}

void AChunk::DestroyAuxiliary(AVoxelAuxiliary* InAuxiliary)
{
	if(!InAuxiliary || !InAuxiliary->IsValidLowLevel()) return;

	if(VoxelMap.Contains(InAuxiliary->GetVoxelItem().Index))
	{
		InAuxiliary->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		InAuxiliary->Destroy();
		InAuxiliary->GetVoxelItem().Auxiliary = nullptr;
	}
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
		default: break;
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

void AChunk::AttachPickUp(APickUp* InPickUp)
{
	if(!InPickUp || !InPickUp->IsValidLowLevel() || PickUps.Contains(InPickUp)) return;

	InPickUp->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	InPickUp->OwnerChunk = this;
	PickUps.Add(InPickUp);
}

void AChunk::DetachPickUp(APickUp* InPickUp)
{
	if(!InPickUp || !InPickUp->IsValidLowLevel() || !PickUps.Contains(InPickUp)) return;

	InPickUp->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	InPickUp->OwnerChunk = nullptr;
	PickUps.Remove(InPickUp);
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

ADWCharacter* AChunk::SpawnCharacter(FCharacterData InSaveData)
{
	FActorSpawnParameters spawnParams = FActorSpawnParameters();
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (ADWCharacter* character = GetWorld()->SpawnActor<ADWCharacter>(InSaveData.SpawnClass, spawnParams))
	{
		character->LoadData(InSaveData);
		AttachCharacter(character);
		return character;
	}
	return nullptr;
}

void AChunk::AttachCharacter(ADWCharacter* InCharacter)
{
	if(!InCharacter || !InCharacter->IsValidLowLevel() || Characters.Contains(InCharacter)) return;

	InCharacter->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	InCharacter->SetOwnerChunk(this);
	Characters.Add(InCharacter);
}

void AChunk::DetachCharacter(ADWCharacter* InCharacter)
{
	if(!InCharacter || !InCharacter->IsValidLowLevel() || !Characters.Contains(InCharacter)) return;

	InCharacter->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	InCharacter->SetOwnerChunk(nullptr);
	Characters.Remove(InCharacter);
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

AVitalityObject* AChunk::SpawnVitalityObject(FVitalityObjectData InSaveData)
{
	FActorSpawnParameters spawnParams = FActorSpawnParameters();
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (AVitalityObject* vitalityObject = GetWorld()->SpawnActor<AVitalityObject>(InSaveData.SpawnClass, spawnParams))
	{
		vitalityObject->LoadData(InSaveData);
		AttachVitalityObject(vitalityObject);
		return vitalityObject;
	}
	return nullptr;
}

void AChunk::AttachVitalityObject(AVitalityObject* InVitalityObject)
{
	if(!InVitalityObject || !InVitalityObject->IsValidLowLevel() || VitalityObjects.Contains(InVitalityObject)) return;

	InVitalityObject->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	InVitalityObject->SetOwnerChunk(this);
	VitalityObjects.Add(InVitalityObject);
}

void AChunk::DetachVitalityObject(AVitalityObject* InVitalityObject)
{
	if(!InVitalityObject || !InVitalityObject->IsValidLowLevel() || !VitalityObjects.Contains(InVitalityObject)) return;

	InVitalityObject->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	InVitalityObject->SetOwnerChunk(nullptr);
	VitalityObjects.Remove(InVitalityObject);
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
