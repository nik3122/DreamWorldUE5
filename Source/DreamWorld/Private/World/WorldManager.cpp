// Fill out your copyright notice in the Description page of Project Settings.


#include "World/WorldManager.h"
#include "World/Chunk.h"
#include "Voxel/Voxel.h"
#include "Engine/World.h"
#include "Character/Player/DWPlayerCharacter.h"
#include "Character/Player/DWPlayerCharacterController.h"
#include "PickUp/PickUpVoxel.h"
#include "Voxel/Components/VoxelMeshComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/BoxComponent.h"
#include "Widget/Inventory/WidgetInventoryPanel.h"
#include "Vitality/VitalityObject.h"
#include "DWGameInstance.h"
#include "DataSave/WorldDataSave.h"
#include "Inventory/Character/CharacterInventory.h"
#include "DWGameState.h"
#include "DataSave/PlayerDataSave.h"
#include "World/Components/WorldTimerComponent.h"
#include "World/Components/WorldWeatherComponent.h"
#include "ConstructorHelpers.h"

FWorldData FWorldData::Empty = FWorldData();

AWorldManager* AWorldManager::Current = nullptr;

UWorldDataSave* AWorldManager::DataSave = nullptr;

// Sets default values
AWorldManager::AWorldManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	static ConstructorHelpers::FClassFinder<UWorldTimerComponent> WorldTimerClass(TEXT("Blueprint'/Game/Blueprints/World/BPC_WorldTimer.BPC_WorldTimer_C'"));
	if(WorldTimerClass.Succeeded())
	{
		WorldTimer = Cast<UWorldTimerComponent>(CreateDefaultSubobject(TEXT("WorldTimer"), WorldTimerClass.Class, WorldTimerClass.Class, true, true));
	}
	static ConstructorHelpers::FClassFinder<UWorldWeatherComponent> WeatherTimerClass(TEXT("Blueprint'/Game/Blueprints/World/BPC_WorldWeather.BPC_WorldWeather_C'"));
	if(WeatherTimerClass.Succeeded())
	{                                                                                          
		WorldWeather = Cast<UWorldWeatherComponent>(CreateDefaultSubobject(TEXT("WorldWeather"), WeatherTimerClass.Class, WeatherTimerClass.Class, true, true));
	}

	VoxelsCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("VoxelsCapture"));
	VoxelsCapture->ProjectionType = ECameraProjectionMode::Orthographic;
	VoxelsCapture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
	VoxelsCapture->SetupAttachment(RootComponent);
	VoxelsCapture->SetRelativeLocationAndRotation(FVector(0, 0, -500), FRotator(90, 0, 0));

	BoundsMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoundsMesh"));
	BoundsMesh->SetRelativeScale3D(FVector::ZeroVector);
	BoundsMesh->SetRelativeRotation(FRotator(0, 0, 0));
	BoundsMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	bBasicGenerated = false;
	ChunkSpawnBatch = 0;
	LastGenerateIndex = FIndex::ZeroIndex;
	LastStayChunkIndex = FIndex::ZeroIndex;
	TeamMap = TMap<FName, FTeamData>();
	ChunkMap = TMap<FIndex, AChunk*>();
	ChunkSpawnQueue = TArray<FIndex>();
	ChunkMapBuildQueue = TArray<AChunk*>();
	ChunkGenerateQueue = TArray<AChunk*>();
	ChunkDestroyQueue = TArray<AChunk*>();

	Current = this;
}

// Called when the game starts or when spawned
void AWorldManager::BeginPlay()
{
	Super::BeginPlay();

	if(ADWPlayerCharacterController* PlayerController = UDWHelper::GetPlayerController(this))
	{
		PlayerController->OnPlayerSpawned.AddDynamic(this, &AWorldManager::OnPlayerSpawned);
	}

	//GeneratePreviews();
}

// Called every frame
void AWorldManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (UDWHelper::GetGameState(this)->GetCurrentState())
	{
		case EGameState::Playing:
		{
			GenerateTerrain();
			if(WorldTimer)
			{
				WorldTimer->UpdateTimer();
			}
			if (WorldWeather)
			{
				WorldWeather->UpdateWeather();
			}
			break;
		}
		case EGameState::Loading:
		{
			GenerateTerrain();
			break;
		}
		default: break;
	}
}

void AWorldManager::LoadWorld(const FString& InWorldName)
{
	if(ADWGameState* GameState = UDWHelper::GetGameState(this))
	{
		GameState->SetCurrentState(EGameState::Loading);
		if(UDWGameInstance* GameInstance = UDWHelper::GetGameInstance(this))
		{
			if(UWorldDataSave* WorldDataSave = GameInstance->LoadWorldData(InWorldName))
			{
				DataSave = WorldDataSave;
				WorldData = WorldDataSave->GetWorldData();
				if(WorldData.WorldSeed == 0)
				{
					WorldData.WorldSeed = FMath::Rand();
				}
				RandomStream = FRandomStream(WorldData.WorldSeed);
				UDWHelper::Debug(FString::FromInt(WorldData.WorldSeed), EDebugType::Console);
				UDWHelper::Debug(FString::SanitizeFloat(RandomStream.FRand()), EDebugType::Console);
			}
		}
	}
}

void AWorldManager::UnloadWorld()
{
	for (auto iter : ChunkMap)
	{
		if(iter.Value)
		{
			iter.Value->Destroy();
		}
	}
	ChunkMap.Empty();

	if(UDWGameInstance* GameInstance = UDWHelper::GetGameInstance(this))
	{
		GameInstance->UnloadWorldData(WorldData.WorldName);
	}

	WorldData = FWorldData();
	
	ChunkSpawnBatch = 0;
	bBasicGenerated = false;
	LastGenerateIndex = FIndex::ZeroIndex;
	LastStayChunkIndex = FIndex::ZeroIndex;

	ChunkSpawnQueue.Empty();
	ChunkMapBuildQueue.Empty();
	ChunkGenerateQueue.Empty();
	ChunkDestroyQueue.Empty();

	TeamMap.Empty();
}

void AWorldManager::InitRandomStream(int32 InDeltaSeed)
{
	RandomStream.Initialize(WorldData.WorldSeed + InDeltaSeed);
}

void AWorldManager::GenerateTerrain()
{
	if(ADWPlayerCharacter* PlayerCharacter = UDWHelper::GetPlayerCharacter(this))
	{
		FIndex chunkIndex = LocationToChunkIndex(PlayerCharacter->GetActorLocation(), true);

		if (FIndex::Distance(chunkIndex, LastGenerateIndex, true) >= WorldData.ChunkSpawnDistance)
		{
			GenerateChunks(chunkIndex);
		}

		if (chunkIndex != LastStayChunkIndex)
		{
			LastStayChunkIndex = chunkIndex;
			for (auto iter = ChunkMap.CreateConstIterator(); iter; ++iter)
			{
				FIndex index = iter->Key;
				AChunk* chunk = iter->Value;
				if (FIndex::Distance(chunkIndex, index, true) >= WorldData.GetChunkDistance())
				{
					AddToDestroyQueue(chunk);
				}
				else if (ChunkDestroyQueue.Contains(chunk))
				{
					ChunkDestroyQueue.Remove(chunk);
				}
			}
		}

		if (ChunkDestroyQueue.Num() > 0)
		{
			int32 tmpNum = FMath::Min(ChunkDestroyQueue.Num(), WorldData.ChunkDestroySpeed);
			for (int32 i = 0; i < tmpNum; i++)
			{
				DestroyChunk(ChunkDestroyQueue[i]);
			}
			ChunkDestroyQueue.RemoveAt(0, tmpNum);
		}
		else if (ChunkSpawnQueue.Num() > 0)
		{
			int32 tmpNum = FMath::Min(ChunkSpawnQueue.Num(), WorldData.ChunkSpawnSpeed);
			for (int32 i = 0; i < tmpNum; i++)
			{
				SpawnChunk(ChunkSpawnQueue[i]);
			}
			ChunkSpawnQueue.RemoveAt(0, tmpNum);
		}
		else if (ChunkMapBuildQueue.Num() > 0)
		{
			int32 tmpNum = FMath::Min(ChunkMapBuildQueue.Num(), WorldData.ChunkMapBuildSpeed);
			for (int32 i = 0; i < tmpNum; i++)
			{
				BuildChunkMap(ChunkMapBuildQueue[i]);
			}
			ChunkMapBuildQueue.RemoveAt(0, tmpNum);
		}
		else if (ChunkGenerateQueue.Num() > 0)
		{
			int32 tmpNum = FMath::Min(ChunkGenerateQueue.Num(), WorldData.ChunkGenerateSpeed);
			for (int32 i = 0; i < tmpNum; i++)
			{
				GenerateChunk(ChunkGenerateQueue[i]);
			}
			ChunkGenerateQueue.RemoveAt(0, tmpNum);
		}

		if (!bBasicGenerated)
		{
			FHitResult hitResult;
			FVector rayStart = FVector(PlayerCharacter->GetActorLocation().X, PlayerCharacter->GetActorLocation().Y, WorldData.ChunkHeightRange * WorldData.GetChunkLength() + 500);
			FVector rayEnd = FVector(PlayerCharacter->GetActorLocation().X, PlayerCharacter->GetActorLocation().Y, 0);
			if (ChunkTraceSingle(rayStart, rayEnd, PlayerCharacter->GetRadius(), PlayerCharacter->GetHalfHeight(), hitResult))
			{
				bBasicGenerated = true;

				OnBasicGenerated.Broadcast(hitResult.Location);
				
				if(ADWGameState* GameState = UDWHelper::GetGameState(this))
				{
					GameState->SetCurrentState(EGameState::Playing);
				}
			}
		}
	}
}

void AWorldManager::OnPlayerSpawned(ADWPlayerCharacter* InPlayerCharacter)
{
	GenerateChunks(LocationToChunkIndex(InPlayerCharacter->GetActorLocation(), true));
}

void AWorldManager::GeneratePreviews()
{
	auto voxelDatas = UDWHelper::LoadVoxelDatas();
	if(voxelDatas.Num() == 0) return;

	int32 tmpNum = 8;
	int32 tmpIndex = 0;
	VoxelsCapture->OrthoWidth = tmpNum * WorldData.BlockSize * 0.5f;
	for (float x = -(tmpNum - 1) * 0.5f; x <= (tmpNum - 1) * 0.5f; x++)
	{
		for (float y = -(tmpNum - 1) * 0.5f; y <= (tmpNum - 1) * 0.5f; y++)
		{
			if (tmpIndex >= voxelDatas.Num()) break;

			//voxelDatas[tmpIndex].TexUV = FVector2D((tmpIndex - 1) % tmpNum, (tmpIndex - 1) / tmpNum);
			FActorSpawnParameters spawnParams = FActorSpawnParameters();
			spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			auto pickUpItem = GetWorld()->SpawnActor<APickUpVoxel>(spawnParams);
			if (pickUpItem != nullptr)
			{
				pickUpItem->Initialize(FItem(voxelDatas[tmpIndex].ID), true);
				pickUpItem->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
				pickUpItem->SetActorLocation(FVector(x * WorldData.BlockSize * 0.5f, y * WorldData.BlockSize * 0.5f, 0));
				pickUpItem->SetActorRotation(FRotator(-70, 0, -180));
				pickUpItem->GetMeshComponent()->SetRelativeRotation(FRotator(0, 45, 0));
				VoxelsCapture->ShowOnlyActors.Add(pickUpItem);
			}

			tmpIndex++;
		}
	}
}

void AWorldManager::GenerateChunks(FIndex InIndex)
{
	if (ChunkSpawnQueue.Num() > 0) return;

	for (int32 x = -WorldData.ChunkSpawnRange + InIndex.X; x < WorldData.ChunkSpawnRange + InIndex.X; x++)
	{
		for (int32 y = -WorldData.ChunkSpawnRange + InIndex.Y; y < WorldData.ChunkSpawnRange + InIndex.Y; y++)
		{
			for (int32 z = 0; z < WorldData.ChunkHeightRange ; z++)
			{
				AddToSpawnQueue(FIndex(x, y, z));
			}
		}
	}

	ChunkSpawnQueue.Sort([InIndex](const FIndex& A, const FIndex& B) {
		float lengthA = FIndex::Distance(InIndex, A, true);
		float lengthB = FIndex::Distance(InIndex, B, true);
		if (lengthA == lengthB)
			return A.Z < B.Z;
		return lengthA < lengthB;
	});

	if(BoundsMesh != nullptr)
	{
		BoundsMesh->SetRelativeLocation(ChunkIndexToLocation(InIndex));
		BoundsMesh->SetRelativeScale3D(FVector(WorldData.GetWorldLength() * WorldData.BlockSize * 0.01f, WorldData.GetWorldLength() * WorldData.BlockSize * 0.01f, 15.f));
	}

	LastGenerateIndex = InIndex;
	ChunkSpawnBatch++;
}

void AWorldManager::BuildChunkMap(AChunk* InChunk)
{
	if (!InChunk || !ChunkMap.Contains(InChunk->GetIndex())) return;

	if(UDWGameInstance* GameInstance = UDWHelper::GetGameInstance(this))
	{
		if (UWorldDataSave* WorldDataSave = GameInstance->LoadWorldData(WorldData.WorldName))
		{
			if (WorldDataSave->IsExistChunkData(InChunk->GetIndex()))
			{
				InChunk->LoadMap(WorldDataSave->LoadChunkData(InChunk->GetIndex()));
			}
			else
			{
				InChunk->BuildMap();
			}
		}
	}

	AddToGenerateQueue(InChunk);
}

void AWorldManager::GenerateChunk(AChunk* InChunk)
{
	if (!InChunk || !ChunkMap.Contains(InChunk->GetIndex())) return;
	
	InChunk->Generate();
}

void AWorldManager::DestroyChunk(AChunk* InChunk)
{
	if (!InChunk || !ChunkMap.Contains(InChunk->GetIndex())) return;

	InChunk->Destroy();

	if (ChunkMapBuildQueue.Contains(InChunk))
	{
		ChunkMapBuildQueue.Remove(InChunk);
	}

	if (ChunkGenerateQueue.Contains(InChunk))
	{
		ChunkGenerateQueue.Remove(InChunk);
	}

	ChunkMap.Remove(InChunk->GetIndex());
}

void AWorldManager::AddToSpawnQueue(FIndex InIndex)
{
	if (!ChunkMap.Contains(InIndex) && !ChunkSpawnQueue.Contains(InIndex))
	{
		ChunkSpawnQueue.Add(InIndex);
	}
}

void AWorldManager::AddToBuildMapQueue(AChunk* InChunk)
{
	if (ChunkMap.Contains(InChunk->GetIndex()) && !ChunkMapBuildQueue.Contains(InChunk))
	{
		ChunkMapBuildQueue.Add(InChunk);
	}
}

void AWorldManager::AddToGenerateQueue(AChunk* InChunk)
{
	if (ChunkMap.Contains(InChunk->GetIndex()) && !ChunkGenerateQueue.Contains(InChunk))
	{
		ChunkGenerateQueue.Add(InChunk);
	}
}

void AWorldManager::AddToDestroyQueue(AChunk* InChunk)
{
	if (ChunkMap.Contains(InChunk->GetIndex()) && !ChunkDestroyQueue.Contains(InChunk))
	{
		ChunkDestroyQueue.Add(InChunk);
	}
}

AChunk* AWorldManager::SpawnChunk(FIndex InIndex, bool bAddToQueue)
{
	if (ChunkMap.Contains(InIndex)) return ChunkMap[InIndex];

	FActorSpawnParameters spawnParams = FActorSpawnParameters();
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	auto chunk = GetWorld()->SpawnActor<AChunk>(InIndex.ToVector() * WorldData.GetChunkLength(), FRotator::ZeroRotator, spawnParams);
	if (chunk)
	{
		ChunkMap.Add(InIndex, chunk);

		chunk->Initialize(InIndex, ChunkSpawnBatch);
		chunk->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);

		if (bAddToQueue)
		{
			AddToBuildMapQueue(chunk);
		}
	}
	return chunk;
}

AChunk* AWorldManager::FindChunk(FVector InLocation)
{
	return FindChunk(LocationToChunkIndex(InLocation));
}

AChunk* AWorldManager::FindChunk(FIndex InIndex)
{
	if (ChunkMap.Contains(InIndex))
	{
		return ChunkMap[InIndex];
	}
	return nullptr;
}

EVoxelType AWorldManager::GetNoiseVoxelType(FIndex InIndex) const
{
	const FVector offsetIndex = FVector(InIndex.X + WorldData.WorldSeed, InIndex.Y + WorldData.WorldSeed, InIndex.Z);
	
	const int plainHeight = GetNoiseTerrainHeight(offsetIndex, WorldData.TerrainPlainScale);
	const int mountainHeight = GetNoiseTerrainHeight(offsetIndex, WorldData.TerrainMountainScale);
	
	const int baseHeight = FMath::Clamp(FMath::Max(plainHeight, mountainHeight) + int32(WorldData.GetWorldHeight() * WorldData.TerrainBaseHeight), 0, WorldData.GetWorldHeight() - 1);

	const int stoneHeight = FMath::Clamp(GetNoiseTerrainHeight(offsetIndex, WorldData.TerrainStoneVoxelScale), 0, WorldData.GetWorldHeight() - 1);
	const int sandHeight = FMath::Clamp(GetNoiseTerrainHeight(offsetIndex, WorldData.TerrainSandVoxelScale), 0, WorldData.GetWorldHeight() - 1);

	const int waterHeight = FMath::Clamp(int32(WorldData.GetWorldHeight() * WorldData.TerrainWaterVoxelScale), 0, WorldData.GetWorldHeight() - 1);
	const int bedrockHeight = FMath::Clamp(int32(WorldData.GetWorldHeight() * WorldData.TerrainBedrockVoxelScale), 0, WorldData.GetWorldHeight() - 1);
	
	if (InIndex.Z < baseHeight)
	{
		if (InIndex.Z <= bedrockHeight)
		{
			return EVoxelType::Bedrock; //Bedrock
		}
		else if (InIndex.Z <= stoneHeight)
		{
			return EVoxelType::Stone; //Stone
		}
		return EVoxelType::Dirt; //Dirt
	}
	else if(InIndex.Z <= sandHeight)
	{
		return EVoxelType::Sand; //Sand
	}
	else if (InIndex.Z <= waterHeight)
	{
		return EVoxelType::Water; //Water
	}
	else if (InIndex.Z == baseHeight)
	{
		return EVoxelType::Grass; //Grass
	}
	return EVoxelType::Empty; //Empty
}

FVoxelData AWorldManager::GetNoiseVoxelData(FIndex InIndex)
{
	return UDWHelper::LoadVoxelData(*FString::Printf(TEXT("Voxel_%d"), (int32)GetNoiseVoxelType(InIndex)));
}

int AWorldManager::GetNoiseTerrainHeight(FVector InOffset, FVector InScale) const
{
	return (FMath::PerlinNoise2D(FVector2D(InOffset.X * InScale.X, InOffset.Y * InScale.Y)) + 1) * WorldData.GetWorldHeight() * InScale.Z;
}

FIndex AWorldManager::LocationToChunkIndex(FVector InLocation, bool bIgnoreZ /*= false*/) const
{
	FIndex chunkIndex = FIndex(FMath::FloorToInt(InLocation.X / WorldData.GetChunkLength()),
		FMath::FloorToInt(InLocation.Y / WorldData.GetChunkLength()),
		bIgnoreZ ? 0 : FMath::FloorToInt(InLocation.Z / WorldData.GetChunkLength()));
	return chunkIndex;
}

FVector AWorldManager::ChunkIndexToLocation(FIndex InIndex) const
{
	return InIndex.ToVector() * WorldData.GetChunkLength();
}

int32 AWorldManager::GetChunkNum(bool bNeedGenerated /*= false*/) const
{
	int32 num = 0;
	for (auto iter = ChunkMap.CreateConstIterator(); iter; ++iter)
	{
		if(AChunk* chunk = iter->Value)
		{
			if (!bNeedGenerated || chunk->IsGenerated())
			{
				num++;
			}
		}
	}
	return num;
}

bool AWorldManager::ChunkTraceSingle(AChunk* InChunk, float InRadius, float InHalfHeight, FHitResult& OutHitResult)
{
	FVector rayStart = InChunk->GetActorLocation() + FVector(FMath::FRandRange(1, WorldData.ChunkSize - 1), FMath::FRandRange(1, WorldData.ChunkSize), WorldData.ChunkSize) * WorldData.BlockSize;
	rayStart.X = ((int32)(rayStart.X / WorldData.BlockSize) + 0.5f) * WorldData.BlockSize;
	rayStart.Y = ((int32)(rayStart.Y / WorldData.BlockSize) + 0.5f) * WorldData.BlockSize;
	FVector rayEnd = rayStart + FVector::DownVector * WorldData.GetChunkLength();
	return ChunkTraceSingle(rayStart, rayEnd, InRadius * 0.95f, InHalfHeight * 0.95f, OutHitResult);
}

bool AWorldManager::ChunkTraceSingle(FVector RayStart, FVector RayEnd, float InRadius, float InHalfHeight, FHitResult& OutHitResult)
{
	if (UKismetSystemLibrary::CapsuleTraceSingle(this, RayStart, RayEnd, InRadius, InHalfHeight, UDWHelper::GetGameTrace(EGameTraceType::Chunk), false, TArray<AActor*>(), EDrawDebugTrace::None, OutHitResult, true))
		return OutHitResult.ImpactPoint.Z > 0;
	return false;
}

bool AWorldManager::VoxelTraceSingle(UVoxel* InVoxel, FVector InPoint, FHitResult& OutHitResult)
{
	FVector size = InVoxel->GetVoxelData().GetCeilRange(InVoxel) * WorldData.BlockSize * 0.5f;
	return UKismetSystemLibrary::BoxTraceSingle(this, InPoint + size, InPoint + size, size * 0.95f, FRotator::ZeroRotator, UDWHelper::GetGameTrace(EGameTraceType::Voxel), false, TArray<AActor*>(), EDrawDebugTrace::None, OutHitResult, true);
}

bool AWorldManager::IsExistTeam(const FName& InTeamID) const
{
	return TeamMap.Contains(InTeamID);
}

bool AWorldManager::CreateTeam(ADWCharacter* InCaptain, FName InTeamName /*= NAME_None*/, FString InTeamDetail /*= TEXT("")*/)
{
	if (InCaptain->GetTeamID().IsEmpty())
	{
		auto tmpData = FTeamData();
		tmpData.ID = *FString::Printf(TEXT("Team_%d"), TeamMap.Num());
		if (!IsExistTeam(tmpData.ID))
		{
			tmpData.Name = !InTeamName.IsNone() ? InTeamName : *FString::Printf(TEXT("%s �� Team"), *InCaptain->GetName());
			tmpData.Detail = !InTeamDetail.IsEmpty() ? InTeamDetail : tmpData.Name.ToString();
			tmpData.Captain = InCaptain;
			tmpData.AddMember(InCaptain);
			TeamMap.Add(tmpData.ID, tmpData);
			return true;
		}
	}
	return false;
}

bool AWorldManager::DissolveTeam(const FName& InTeamID, ADWCharacter* InCaptain)
{
	if (IsExistTeam(InTeamID) && TeamMap[InTeamID].IsCaptain(InCaptain))
	{
		TeamMap[InTeamID].DissolveTeam();
		TeamMap.Remove(InTeamID);
		return true;
	}
	return false;
}

FTeamData* AWorldManager::GetTeamData(const FName& InTeamID)
{
	if (TeamMap.Contains(InTeamID))
	{
		return &TeamMap[InTeamID];
	}
	return &FTeamData::Empty;
}
