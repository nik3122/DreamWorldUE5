// Fill out your copyright notice in the Description page of Project Settings.PickUp/


#include "World/Chunk.h"

#include "DWCharacterPart.h"
#include "Voxel/Voxel.h"
#include "PickUp/PickUp.h"
#include "PickUp/PickUpVoxel.h"
#include "Components/BoxComponent.h"
#include "Character/Player/DWPlayerCharacter.h"
#include "World/WorldManager.h"
#include "Voxel/Components/VoxelMeshComponent.h"
#include "Vitality/VitalityObject.h"
#include "Gameplay/DWGameInstance.h"
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
	if(OtherActor && OtherActor->IsA(ADWCharacter::StaticClass()))
	{
		if(ADWCharacter* character = Cast<ADWCharacter>(OtherActor))
		{
			const FVoxelItem& voxelItem = GetVoxelItem(LocationToIndex(Hit.ImpactPoint - AWorldManager::GetWorldData().GetBlockSizedNormal(Hit.ImpactNormal)));
			if (voxelItem.IsValid())
			{
				if(UVoxel* voxel = voxelItem.GetVoxel())
				{
					voxel->OnTargetHit(character, FVoxelHitResult(voxelItem, Hit.ImpactPoint, Hit.ImpactNormal));
					UVoxel::DespawnVoxel(voxel);
				}
			}
		}
	}
}

void AChunk::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// if(OtherComp && OtherComp->IsA(UDWCharacterPart::StaticClass()))
	// {
	// 	if(UDWCharacterPart* characterPart = Cast<UDWCharacterPart>(OtherComp))
	// 	{
	// 		if(ADWCharacter* character = characterPart->GetOwnerCharacter())
	// 		{
	// 			Debug(FString::Printf(TEXT("%s"), *SweepResult.ImpactPoint.ToString()));
	// 			Debug(FString::Printf(TEXT("%s"), *SweepResult.ImpactNormal.ToString()));
	// 			const FVector normal = FVector(FMath::Min((character->GetMoveDirection(false) * 2.5f).X, 1.f), FMath::Min((character->GetMoveDirection(false) * 2.5f).Y, 1.f), FMath::Min((character->GetMoveDirection(false) * 2.5f).Z, 1.f));
	// 			const FVector point = characterPart->GetComponentLocation() + characterPart->GetScaledCapsuleRadius() * normal + AWorldManager::GetWorldData().GetBlockSizedNormal(normal);
	// 			if(FVector::Distance(character->GetActorLocation(), point) > 100)
	// 			{
	// 				Debug(FString::Printf(TEXT("%f"), FVector::Distance(character->GetActorLocation(), point)));
	// 			}
	// 			UKismetSystemLibrary::DrawDebugBox(this, point, FVector(5), FColor::Green, FRotator::ZeroRotator, 100);
	// 			const FVoxelItem& voxelItem = GetVoxelItem(LocationToIndex(point));
	// 			const FVoxelData voxelData = voxelItem.GetVoxelData();
	// 			if(voxelItem.IsValid() && voxelData.VoxelType != characterPart->GetLastOverlapVoxel())
	// 			{
	// 				if(UVoxel* voxel = voxelItem.GetVoxel())
	// 				{
	// 					characterPart->SetLastVoxelType(voxelData.VoxelType);
	// 					voxel->OnTargetEnter(characterPart, FVoxelHitResult(voxelItem, point, normal));
	// 					UVoxel::DespawnVoxel(voxel);
	// 				}
	// 			}
	// 			character->OnBeginOverlapVoxel(characterPart, FVoxelHitResult(voxelItem, point, normal));
	// 		}
	// 	}
	// }
}

void AChunk::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// if(OtherComp && OtherComp->IsA(UDWCharacterPart::StaticClass()))
	// {
	// 	if(UDWCharacterPart* characterPart = Cast<UDWCharacterPart>(OtherComp))
	// 	{
	// 		if(ADWCharacter* character = characterPart->GetOwnerCharacter())
	// 		{
	// 			const FVector normal = FVector(FMath::Min((character->GetMoveDirection(false) * 2.5f).X, 1.f), FMath::Min((character->GetMoveDirection(false) * 2.5f).Y, 1.f), FMath::Min((character->GetMoveDirection(false) * 2.5f).Z, 1.f));
	// 			const FVector point = characterPart->GetComponentLocation() - characterPart->GetScaledCapsuleRadius() * normal - AWorldManager::GetWorldData().GetBlockSizedNormal(normal);
	// 			UKismetSystemLibrary::DrawDebugBox(this, point, FVector(5), FColor::Red, FRotator::ZeroRotator, 100);
	// 			const FVoxelItem& voxelItem = GetVoxelItem(LocationToIndex(point));
	// 			const FVoxelData voxelData = voxelItem.GetVoxelData();
	// 			if(voxelItem.IsValid() && voxelData.VoxelType == characterPart->GetLastOverlapVoxel())
	// 			{
	// 				const FVoxelItem& tmpVoxelItem = GetVoxelItem(LocationToIndex(characterPart->GetComponentLocation()));
	// 				const FVoxelData tmpVoxelData = tmpVoxelItem.GetVoxelData();
	// 				if(!tmpVoxelItem.IsValid() || tmpVoxelData.VoxelType != voxelData.VoxelType)
	// 				{
	// 					if(UVoxel* voxel = voxelItem.GetVoxel())
	// 					{
	// 						characterPart->SetLastVoxelType(EVoxelType::Unknown);
	// 						voxel->OnTargetExit(characterPart, FVoxelHitResult(voxelItem, point, normal));
	// 						UVoxel::DespawnVoxel(voxel);
	// 					}
	// 				}
	// 			}
	// 			character->OnEndOverlapVoxel(characterPart, FVoxelHitResult(voxelItem, point, normal));
	// 		}
	// 	}
	// }
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
		SemiMesh->OnComponentHit.AddDynamic(this, &AChunk::OnCollision);
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
		TransMesh->OnComponentBeginOverlap.AddDynamic(this, &AChunk::OnBeginOverlap);
		TransMesh->OnComponentEndOverlap.AddDynamic(this, &AChunk::OnEndOverlap);
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
		const FVoxelItem& voxelItem = iter.Value;
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
	for (int32 x = 0; x < AWorldManager::GetWorldData().ChunkSize; x++)
	{
		for (int32 y = 0; y < AWorldManager::GetWorldData().ChunkSize; y++)
		{
			for (int32 z = 0; z < AWorldManager::GetWorldData().ChunkSize; z++)
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
								SetVoxelComplex(FIndex(x, y, z + 1), FVoxelItem(FMath::FRandRange(0, 1) > 0.2f ? EVoxelType::Tall_Grass : (EVoxelType)FMath::RandRange((int32)EVoxelType::Flower_Allium, (int32)EVoxelType::Flower_Tulip_White)));
							}
							// tree
							else if (tmpNum < 0.21f)
							{
								if ((x > 2 && x <= AWorldManager::GetWorldData().ChunkSize - 2) && (y > 2 && y <= AWorldManager::GetWorldData().ChunkSize - 2))
								{
									const int32 treeHeight = FMath::RandRange(4, 5);
									const int32 leavesHeight = 2/*FMath::RandRange(2, 2)*/;
									const int32 leavesWidth = FMath::FRandRange(0, 1) < 0.5f ? 3 : 5;
									for (int32 trunkHeight = 0; trunkHeight < treeHeight; trunkHeight++)
									{
										SetVoxelComplex(FIndex(x, y, z + trunkHeight + 1), FVoxelItem(EVoxelType::Oak));
									}
									for (int32 offsetZ = treeHeight - leavesHeight; offsetZ < treeHeight + 1; offsetZ++)
									{
										for (int32 offsetX = -leavesWidth / 2; offsetX <= leavesWidth / 2; offsetX++)
										{
											for (int32 offsetY = -leavesWidth / 2; offsetY <= leavesWidth / 2; offsetY++)
											{
												const FVoxelItem& voxelItem = GetVoxelItem(x + offsetX, y + offsetY, z + offsetZ + 1);
												if (!voxelItem.IsValid() || voxelItem.GetVoxelData().Transparency == ETransparency::Transparent)
												{
													SetVoxelComplex(FIndex(x + offsetX, y + offsetY, z + offsetZ + 1), FVoxelItem(EVoxelType::Oak_Leaves));
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
					SetVoxelSample(voxelIndex, FVoxelItem(voxelType));
				}
			}
		}
	}
}

void AChunk::LoadMap(FChunkData InChunkData)
{
	for (int32 i = 0; i < InChunkData.VoxelItems.Num(); i++)
	{
		const FVoxelItem& voxelItem = InChunkData.VoxelItems[i];
		SetVoxelComplex(voxelItem.Index, voxelItem);
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
		else if(SolidMesh || SemiMesh)
		{
			if (FIndex::Distance(Index, AWorldManager::GetWorldData().LastVitalityRaceIndex) > 250.f / AWorldManager::GetWorldData().VitalityRaceDensity)
			{
				auto raceData = UDWHelper::RandomVitalityRaceData();
				for (int32 i = 0; i < raceData.Items.Num(); i++)
				{
					auto vitalityItem = raceData.Items[i];
					auto vitalityData = UDWHelper::LoadVitalityData(vitalityItem.ID);
					for (int32 j = 0; j < vitalityItem.Count; j++)
					{
						for (int32 k = 0; k < 10; k++)
						{
							FHitResult hitResult;
							if (AWorldManager::Get()->ChunkTraceSingle(this, FMath::Max(vitalityData.Range.X, vitalityData.Range.Y) * 0.5f * AWorldManager::GetWorldData().BlockSize, vitalityData.Range.Z * 0.5f * AWorldManager::GetWorldData().BlockSize, hitResult))
							{
								auto saveData = FVitalityObjectSaveData();
								saveData.ID = vitalityData.ID;
								saveData.Name = vitalityData.Name.ToString();
								saveData.RaceID = raceData.ID.ToString();
								saveData.Level = vitalityItem.Level;
								saveData.SpawnLocation = hitResult.Location;
								saveData.SpawnRotation = FRotator(0, FMath::RandRange(0, 360), 0);
								SpawnVitalityObject(saveData);
								AWorldManager::GetWorldData().LastVitalityRaceIndex = Index;
								break;
							}
						}
					}
				}
			}
			if (FIndex::Distance(Index, AWorldManager::GetWorldData().LastCharacterRaceIndex) > 300.f / AWorldManager::GetWorldData().CharacterRaceDensity)
			{
				ADWCharacter* captain = nullptr;
				auto raceData = UDWHelper::RandomCharacterRaceData();
				for (int32 i = 0; i < raceData.Items.Num(); i++)
				{
					auto characterItem = raceData.Items[i];
					auto characterData = UDWHelper::LoadCharacterData(characterItem.ID);
					for (int32 j = 0; j < characterItem.Count; j++)
					{
						for (int32 k = 0; k < 10; k++)
						{
							FHitResult hitResult;
							if (AWorldManager::Get()->ChunkTraceSingle(this, FMath::Max(characterData.Range.X, characterData.Range.Y) * 0.5f * AWorldManager::GetWorldData().BlockSize, characterData.Range.Z * 0.5f * AWorldManager::GetWorldData().BlockSize, hitResult))
							{
								auto saveData = FCharacterSaveData();
								saveData.ID = characterData.ID;
								saveData.Name = characterData.Name.ToString();
								saveData.RaceID = raceData.ID.ToString();
								saveData.Level = characterItem.Level;
								saveData.SpawnLocation = hitResult.Location;
								saveData.SpawnRotation = FRotator(0, FMath::RandRange(0, 360), 0);
								if(auto character = SpawnCharacter(saveData))
								{
									if (captain == nullptr)
									{
										captain = character;
										AWorldManager::Get()->CreateTeam(captain);
									}
									else
									{
										captain->AddTeamMate(character);
									}
								}
								AWorldManager::GetWorldData().LastCharacterRaceIndex = Index;
								break;
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

void AChunk::GenerateNeighbors(int32 InX, int32 InY, int32 InZ)
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
	for (int32 i = 0; i < 6; i++)
	{
		Neighbors[i] = AWorldManager::Get()->FindChunk(Index + UDWHelper::DirectionToIndex((EDirection)i));
		if (Neighbors[i] != nullptr)
		{
			Neighbors[i]->Neighbors[(int32)UDWHelper::InvertDirection((EDirection)i)] = this;
		}
	}
}

void AChunk::BreakNeighbors()
{
	for (int32 i = 0; i < 6; i++)
	{
		if (Neighbors[i] != nullptr)
		{
			Neighbors[i]->Neighbors[(int32)UDWHelper::InvertDirection((EDirection)i)] = nullptr;
			Neighbors[i] = nullptr;
		}
	}
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
	const FVector point = (!bWorldSpace ? InLocation : GetActorTransform().InverseTransformPosition(InLocation)) / AWorldManager::GetWorldData().BlockSize;

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

UVoxel* AChunk::GetVoxel(FIndex InIndex)
{
	return GetVoxel(InIndex.X, InIndex.Y, InIndex.Z);
}

UVoxel* AChunk::GetVoxel(int32 InX, int32 InY, int32 InZ)
{
	return GetVoxelItem(InX, InY, InZ).GetVoxel();
}

FVoxelItem& AChunk::GetVoxelItem(FIndex InIndex)
{
	return GetVoxelItem(InIndex.X, InIndex.Y, InIndex.Z);
}

FVoxelItem& AChunk::GetVoxelItem(int32 InX, int32 InY, int32 InZ)
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
		for (int32 x = 0; x < InRange.X; x++)
		{
			for (int32 y = 0; y < InRange.Y; y++)
			{
				for (int32 z = 0; z < InRange.Z; z++)
				{
					const FVoxelItem& voxelItem = GetVoxelItem(InIndex + FIndex(x, y, z));
					if (voxelItem.IsValid() && (!bIgnoreTransparent || voxelItem.GetVoxelData().Transparency != ETransparency::Transparent))
					{
						return true;
					}
				}
			}
		}
	}
	else
	{
		const FVoxelItem& voxelItem = GetVoxelItem(InIndex);
		if (voxelItem.IsValid() && (!bIgnoreTransparent || voxelItem.GetVoxelData().Transparency != ETransparency::Transparent))
		{
			return true;
		}
	}
	return false;
}

bool AChunk::CheckVoxel(int32 InX, int32 InY, int32 InZ, FVector InRange/* = FVector::OneVector*/, bool bIgnoreTransparent /*= true*/)
{
	return CheckVoxel(FIndex(InX, InY, InZ), InRange, bIgnoreTransparent);
}

bool AChunk::CheckAdjacent(const FVoxelItem& InVoxelItem, EDirection InDirection)
{
	if(InDirection == EDirection::Down && LocalIndexToWorld(InVoxelItem.Index).Z == 0) return false;

	FVoxelData voxelData = InVoxelItem.GetVoxelData();
	
	FVoxelItem& adjacentItem = GetVoxelItem(UDWHelper::GetAdjacentIndex(InVoxelItem.Index, InDirection, InVoxelItem.Rotation));
	FVoxelData adjacentData = adjacentItem.GetVoxelData();

	if (adjacentItem.IsValid())
	{
		if(voxelData.Range * InVoxelItem.Scale == adjacentData.Range * adjacentItem.Scale)
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

bool AChunk::CheckNeighbors(const FVoxelItem& InVoxelItem, EVoxelType InVoxelType, bool bIgnoreBottom, int32 InDistance)
{
	const FVector range = InVoxelItem.GetVoxelData().GetCeilRange(InVoxelItem.Rotation, InVoxelItem.Scale);
	for (int32 x = -InDistance; x < range.X + InDistance; x++)
	{
		for (int32 y = -InDistance; y < range.Y + InDistance; y++)
		{
			for (int32 z = bIgnoreBottom ? 0 : -InDistance; z < range.Z + InDistance; z++)
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

bool AChunk::SetVoxelSample(FIndex InIndex, const FVoxelItem& InVoxelItem, bool bGenerateMesh)
{
	bool RetValue = false;

	if (InVoxelItem.IsValid())
	{
		if (!CheckVoxel(InIndex))
		{
			FVoxelItem VoxelItem;
			VoxelItem.ID = InVoxelItem.ID;
			VoxelItem.Index = InIndex;
			VoxelItem.Owner = this;
			if (!VoxelMap.Contains(InIndex))
			{
				VoxelMap.Add(InIndex, VoxelItem);
			}
			else
			{
				//DestroyVoxel(InIndex);
				VoxelMap[InIndex] = VoxelItem;
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

	return RetValue;
}

bool AChunk::SetVoxelSample(int32 InX, int32 InY, int32 InZ, const FVoxelItem& InVoxelItem, bool bGenerateMesh)
{
	return SetVoxelSample(FIndex(InX, InY, InZ), InVoxelItem, bGenerateMesh);
}

bool AChunk::SetVoxelComplex(FIndex InIndex, const FVoxelItem& InVoxelItem, bool bGenerateMesh)
{
	return SetVoxelComplex(InIndex.X, InIndex.Y, InIndex.Z, InVoxelItem, bGenerateMesh);
}

bool AChunk::SetVoxelComplex(int32 InX, int32 InY, int32 InZ, const FVoxelItem& InVoxelItem, bool bGenerateMesh)
{
	if (InX < 0) {
		if (Neighbors[(int32)EDirection::Back] != nullptr)
			return Neighbors[(int32)EDirection::Back]->SetVoxelComplex(InX + AWorldManager::GetWorldData().ChunkSize, InY, InZ, InVoxelItem, bGenerateMesh);
	}
	else if (InX >= AWorldManager::GetWorldData().ChunkSize) {
		if (Neighbors[(int32)EDirection::Forward] != nullptr)
			return Neighbors[(int32)EDirection::Forward]->SetVoxelComplex(InX - AWorldManager::GetWorldData().ChunkSize, InY, InZ, InVoxelItem, bGenerateMesh);
	}
	else if (InY < 0) {
		if (Neighbors[(int32)EDirection::Left] != nullptr)
			return Neighbors[(int32)EDirection::Left]->SetVoxelComplex(InX, InY + AWorldManager::GetWorldData().ChunkSize, InZ, InVoxelItem, bGenerateMesh);
	}
	else if (InY >= AWorldManager::GetWorldData().ChunkSize) {
		if (Neighbors[(int32)EDirection::Right] != nullptr)
			return Neighbors[(int32)EDirection::Right]->SetVoxelComplex(InX, InY - AWorldManager::GetWorldData().ChunkSize, InZ, InVoxelItem, bGenerateMesh);
	}
	else if (InZ < 0) {
		if (Neighbors[(int32)EDirection::Down] != nullptr)
			return Neighbors[(int32)EDirection::Down]->SetVoxelComplex(InX, InY, InZ + AWorldManager::GetWorldData().ChunkSize, InVoxelItem, bGenerateMesh);
	}
	else if (InZ >= AWorldManager::GetWorldData().ChunkSize) {
		if (Neighbors[(int32)EDirection::Up] == nullptr)
			AWorldManager::Get()->SpawnChunk(Index + UDWHelper::DirectionToIndex(EDirection::Up), !bGenerateMesh);
		if (Neighbors[(int32)EDirection::Up] != nullptr)
			return Neighbors[(int32)EDirection::Up]->SetVoxelComplex(InX, InY, InZ - AWorldManager::GetWorldData().ChunkSize, InVoxelItem, bGenerateMesh);
	}
	else {
		if (InVoxelItem.GetVoxelData().IsComplex())
		{
			if (!InVoxelItem.IsEmpty())
			{
				const FVector range = InVoxelItem.GetVoxelData().GetCeilRange(InVoxelItem.Rotation, InVoxelItem.Scale);
				if (!CheckVoxel(InX, InY, InZ, range))
				{
					for (int32 x = 0; x < range.X; x++)
					{
						for (int32 y = 0; y < range.Y; y++)
						{
							for (int32 z = 0; z < range.Z; z++)
							{
								FIndex index = FIndex(InX + x, InY + y, InZ + z);
								SetVoxelSample(index, InVoxelItem, bGenerateMesh);
							}
						}
					}
					return true;
				}
			}
			else
			{
				const FVoxelItem& voxelItem = GetVoxelItem(InX, InY, InZ);
				if(voxelItem.IsValid())
				{
					FVector range = voxelItem.GetVoxelData().GetCeilRange(voxelItem.Rotation, voxelItem.Scale);
					for (int32 x = 0; x < range.X; x++)
					{
						for (int32 y = 0; y < range.Y; y++)
						{
							for (int32 z = 0; z < range.Z; z++)
							{
								FIndex index = voxelItem.Index + FIndex(InX, InY, InZ);
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
			return SetVoxelSample(InX, InY, InZ, InVoxelItem, bGenerateMesh);
		}
	}
	return false;
}

bool AChunk::GenerateVoxel(FIndex InIndex, const FVoxelItem& InVoxelItem)
{
	if(SetVoxelComplex(InIndex, InVoxelItem, true))
	{
		SpawnAuxiliary(GetVoxelItem(InIndex));
		const FVoxelData VoxelData = InVoxelItem.GetVoxelData();
		if(VoxelData.GenerateSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, VoxelData.GenerateSound, IndexToLocation(InIndex));
		}
		return true;
	}
	return false;
}

bool AChunk::DestroyVoxel(FIndex InIndex)
{
	return DestroyVoxel(GetVoxelItem(InIndex));
}

bool AChunk::DestroyVoxel(const FVoxelItem& InVoxelItem)
{
	if (SetVoxelComplex(InVoxelItem.Index, FVoxelItem::EmptyVoxel, true))
	{
		const FVoxelData voxelData = InVoxelItem.GetVoxelData();
		if(voxelData.GenerateSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, voxelData.GenerateSound, IndexToLocation(InVoxelItem.Index));
		}
		DestroyAuxiliary(InVoxelItem.Auxiliary);

		FVector range = voxelData.GetCeilRange(InVoxelItem.Rotation, InVoxelItem.Scale);
		SpawnPickUp(FItem(InVoxelItem.ID, 1), IndexToLocation(InVoxelItem.Index) + range * AWorldManager::GetWorldData().BlockSize * 0.5f);

		for (int32 x = 0; x < range.X; x++)
		{
			for (int32 y = 0; y < range.Y; y++)
			{
				FIndex tmpIndex = InVoxelItem.Index + FIndex(x, y, range.Z);
				const FVoxelItem& tmpVoxelItem = GetVoxelItem(tmpIndex);
				if (tmpVoxelItem.IsValid())
				{
					FVoxelData tmpVoxelData = tmpVoxelItem.GetVoxelData();
					if(tmpVoxelData.Transparency == ETransparency::Transparent && tmpVoxelData.VoxelType != EVoxelType::Water)
					{
						DestroyVoxel(tmpVoxelItem);
					}
				}
			}
		}
		if (CheckNeighbors(InVoxelItem, EVoxelType::Water, true))
		{
			for (int32 x = 0; x < range.X; x++)
			{
				for (int32 y = 0; y < range.Y; y++)
				{
					for (int32 z = 0; z < range.Z; z++)
					{
						SetVoxelComplex(InVoxelItem.Index + FIndex(x, y, z), FVoxelItem(EVoxelType::Water), true);
					}
				}
			}
		}
		return true;
	}
	return false;
}

bool AChunk::ReplaceVoxel(const FVoxelItem& InOldVoxelItem, const FVoxelItem& InNewVoxelItem)
{
	if (SetVoxelComplex(InOldVoxelItem.Index, InNewVoxelItem, true))
	{
		const FVoxelData VoxelData = InNewVoxelItem.GetVoxelData();
		SpawnAuxiliary(GetVoxelItem(InOldVoxelItem.Index));
		if(VoxelData.GenerateSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, VoxelData.GenerateSound, IndexToLocation(InNewVoxelItem.Index));
		}
		return true;
	}
	return false;
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

APickUp* AChunk::SpawnPickUp(FPickUpSaveData InPickUpData)
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

ADWCharacter* AChunk::SpawnCharacter(FCharacterSaveData InSaveData)
{
	FActorSpawnParameters spawnParams = FActorSpawnParameters();
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (ADWCharacter* character = GetWorld()->SpawnActor<ADWCharacter>(InSaveData.GetCharacterData().Class, spawnParams))
	{
		character->LoadData(InSaveData);
		character->SpawnDefaultController();
		AttachCharacter(character);
		return character;
	}
	return nullptr;
}

void AChunk::AttachCharacter(ADWCharacter* InCharacter)
{
	if(!InCharacter || !InCharacter->IsValidLowLevel() || Characters.Contains(InCharacter)) return;

	// InCharacter->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	InCharacter->SetOwnerChunk(this);
	Characters.Add(InCharacter);
}

void AChunk::DetachCharacter(ADWCharacter* InCharacter)
{
	if(!InCharacter || !InCharacter->IsValidLowLevel() || !Characters.Contains(InCharacter)) return;

	// InCharacter->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
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

AVitalityObject* AChunk::SpawnVitalityObject(FVitalityObjectSaveData InSaveData)
{
	FActorSpawnParameters spawnParams = FActorSpawnParameters();
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (AVitalityObject* vitalityObject = GetWorld()->SpawnActor<AVitalityObject>(InSaveData.GetVitalityData().Class, spawnParams))
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
