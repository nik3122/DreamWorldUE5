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
#include "GameFramework/RotatingMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/BoxComponent.h"
#include "Widget/Inventory/WidgetInventoryPanel.h"
#include "DWGameMode.h"
#include "Vitality/VitalityObject.h"
#include "TimerManager.h"
#include "DWGameInstance.h"
#include "DataSaves/WorldDataSave.h"
#include "Inventory/Character/CharacterInventory.h"
#include "DWGameState.h"
#include "DataSaves/PlayerDataSave.h"
#include "World/Components/WorldTimerComponent.h"
#include "World/Components/WorldWeatherComponent.h"
#include "Engine/DataTable.h"
#include "ConstructorHelpers.h"

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

	WorldName = TEXT("");
	WorldSeed = 0;

	BlockSize = 80;
	ChunkSize = 16;
	ChunkHightRange = 3;
	ChunkSpawnRange = 5;
	ChunkSpawnDistance = 2;

	ChunkSpawnSpeed = 100;
	ChunkDestroySpeed = 100;
	ChunkMapBuildSpeed = 5;
	ChunkGenerateSpeed = 1;

	VitalityRateDensity = 0.2f;
	CharacterRateDensity = 0.2f;

	TerrainBaseHeight = 0.1f;
	TerrainPlainScale = FVector(0.005f, 0.005f, 0.2f);
	TerrainMountainScale = FVector(0.03f, 0.03f, 0.25f);
	TerrainStoneVoxelScale = FVector(0.05f, 0.05f, 0.18f);
	TerrainSandVoxelScale = FVector(0.04f, 0.04f, 0.21f);
	TerrainWaterVoxelScale = 0.3f;
	TerrainBedrockVoxelScale = 0.01f;

	ChunkMaterials = TArray<FChunkMaterial>();

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
}

// Called when the game starts or when spawned
void AWorldManager::BeginPlay()
{
	Super::BeginPlay();
	//GeneratePreviews();
}

// Called every frame
void AWorldManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (UDWHelper::GetGameState()->GetCurrentState())
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
	}
}

void AWorldManager::LoadWorld()
{
	UDWHelper::GetGameState()->SetCurrentState(EGameState::Loading);

	WorldName = UDWHelper::GetWorldDataSave()->GetWorldData().Name;
	WorldSeed = UDWHelper::GetWorldDataSave()->GetWorldData().Seed;
	if(WorldSeed == 0) WorldSeed = FMath::Rand();
	RandomStream = FRandomStream(WorldSeed);
	UDWHelper::Debug(FString::FromInt(WorldSeed), EDebugType::Console);
	UDWHelper::Debug(FString::SanitizeFloat(RandomStream.FRand()), EDebugType::Console);

	FActorSpawnParameters spawnParams = FActorSpawnParameters();
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	auto playerCharacter = GetWorld()->SpawnActor<ADWPlayerCharacter>(UDWHelper::GetPlayerDataSave()->GetPlayerData().Class, spawnParams);

	if (playerCharacter)
	{
		playerCharacter->LoadData(UDWHelper::GetPlayerDataSave()->GetPlayerData());
		UDWHelper::GetPlayerController()->Possess(playerCharacter);

		if (UDWHelper::GetPlayerDataSave()->IsExistWorldRecord(WorldName))
		{
			auto worldRecord = UDWHelper::GetPlayerDataSave()->LoadWorldRecord(WorldName);
			if(WorldTimer) WorldTimer->SetTimeSeconds(worldRecord.TimeSeconds);
			UDWHelper::GetPlayerCharacter()->SetActorLocationAndRotation(worldRecord.PlayerLocation, worldRecord.PlayerRotation);
		}
		else
		{
			if(WorldTimer) WorldTimer->SetTimeSeconds(0);
			playerCharacter->SetActorLocationAndRotation(FVector::ZeroVector, FRotator::ZeroRotator);
		}
		if (!UDWHelper::GetPlayerDataSave()->GetPlayerData().InventoryData.bInitialized)
		{
			auto voxelDatas = UDWHelper::LoadVoxelDatas();
			for (int32 i = 0; i < voxelDatas.Num(); i++)
			{
				FItem tmpItem = FItem(voxelDatas[i].ID, voxelDatas[i].MaxCount);
				playerCharacter->GetInventory()->AdditionItems(tmpItem);
			}

			auto equipDatas = UDWHelper::LoadEquipDatas();
			for (int32 i = 0; i < equipDatas.Num(); i++)
			{
				FItem tmpItem = FItem(equipDatas[i].ID, equipDatas[i].MaxCount);
				playerCharacter->GetInventory()->AdditionItems(tmpItem);
			}

			auto propDatas = UDWHelper::LoadPropDatas();
			for (int32 i = 0; i < propDatas.Num(); i++)
			{
				FItem tmpItem = FItem(propDatas[i].ID, propDatas[i].MaxCount);
				playerCharacter->GetInventory()->AdditionItems(tmpItem);
			}
		}
		GenerateChunks(LocationToChunkIndex(playerCharacter->GetActorLocation(), true));
	}
}

void AWorldManager::UnloadWorld()
{
	for (auto iter = ChunkMap.CreateConstIterator(); iter; ++iter)
	{
		iter->Value->OnDestroy();
	}

	UDWHelper::GetGameInstance()->UnloadPlayerData();
	UDWHelper::GetGameInstance()->UnloadWorldData();

	UDWHelper::GetPlayerCharacter()->Destroy();
	UDWHelper::GetPlayerController()->UnPossess();

	WorldName = TEXT("");
	WorldSeed = 0;
	ChunkSpawnBatch = 0;
	bBasicGenerated = false;
	LastGenerateIndex = FIndex::ZeroIndex;
	LastStayChunkIndex = FIndex::ZeroIndex;

	ChunkMap.Empty();
	TeamMap.Empty();
	ChunkSpawnQueue.Empty();
	ChunkMapBuildQueue.Empty();
	ChunkGenerateQueue.Empty();
	ChunkDestroyQueue.Empty();
}

void AWorldManager::GenerateTerrain()
{
	if(!UDWHelper::GetPlayerCharacter()) return;

	FIndex chunkIndex = LocationToChunkIndex(UDWHelper::GetPlayerCharacter()->GetActorLocation(), true);

	if (FIndex::Distance(chunkIndex, LastGenerateIndex, true) >= ChunkSpawnDistance)
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
			if (FIndex::Distance(chunkIndex, index, true) >= GetChunkDistance())
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
		int32 tmpNum = FMath::Min(ChunkDestroyQueue.Num(), ChunkDestroySpeed);
		for (int32 i = 0; i < tmpNum; i++)
		{
			DestroyChunk(ChunkDestroyQueue[i]);
		}
		ChunkDestroyQueue.RemoveAt(0, tmpNum);
	}
	else if (ChunkSpawnQueue.Num() > 0)
	{
		int32 tmpNum = FMath::Min(ChunkSpawnQueue.Num(), ChunkSpawnSpeed);
		for (int32 i = 0; i < tmpNum; i++)
		{
			SpawnChunk(ChunkSpawnQueue[i]);
		}
		ChunkSpawnQueue.RemoveAt(0, tmpNum);
	}
	else if (ChunkMapBuildQueue.Num() > 0)
	{
		int32 tmpNum = FMath::Min(ChunkMapBuildQueue.Num(), ChunkMapBuildSpeed);
		for (int32 i = 0; i < tmpNum; i++)
		{
			BuildChunkMap(ChunkMapBuildQueue[i]);
		}
		ChunkMapBuildQueue.RemoveAt(0, tmpNum);
	}
	else if (ChunkGenerateQueue.Num() > 0)
	{
		int32 tmpNum = FMath::Min(ChunkGenerateQueue.Num(), ChunkGenerateSpeed);
		for (int32 i = 0; i < tmpNum; i++)
		{
			GenerateChunk(ChunkGenerateQueue[i]);
		}
		ChunkGenerateQueue.RemoveAt(0, tmpNum);
	}

	if (!bBasicGenerated)
	{
		FHitResult hitResult;
		FVector rayStart = FVector(UDWHelper::GetPlayerCharacter()->GetActorLocation().X, UDWHelper::GetPlayerCharacter()->GetActorLocation().Y, ChunkHightRange * GetChunkLength() + 500);
		FVector rayEnd = FVector(UDWHelper::GetPlayerCharacter()->GetActorLocation().X, UDWHelper::GetPlayerCharacter()->GetActorLocation().Y, 0);
		if (ChunkTraceSingle(rayStart, rayEnd, UDWHelper::GetPlayerCharacter()->GetRadius(), UDWHelper::GetPlayerCharacter()->GetHalfHeight(), hitResult))
		{
			OnBasicGenerated(hitResult.Location);
		}
	}
}

void AWorldManager::OnBasicGenerated(FVector InPlayerLocation)
{
	bBasicGenerated = true;
	
	UDWHelper::GetPlayerCharacter()->Active(true);
	UDWHelper::GetGameState()->SetCurrentState(EGameState::Playing);

	if (!UDWHelper::GetPlayerDataSave()->IsExistWorldRecord(WorldName))
	{
		UDWHelper::GetPlayerCharacter()->SetActorLocation(InPlayerLocation);
	}
}

void AWorldManager::GeneratePreviews()
{
	auto voxelDatas = UDWHelper::LoadVoxelDatas();
	if(voxelDatas.Num() == 0) return;

	int32 tmpNum = 8;
	int32 tmpIndex = 0;
	VoxelsCapture->OrthoWidth = tmpNum * BlockSize * 0.5f;
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
				pickUpItem->SetActorLocation(FVector(x * BlockSize * 0.5f, y * BlockSize * 0.5f, 0));
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

	for (int32 x = -ChunkSpawnRange + InIndex.X; x < ChunkSpawnRange + InIndex.X; x++)
	{
		for (int32 y = -ChunkSpawnRange + InIndex.Y; y < ChunkSpawnRange + InIndex.Y; y++)
		{
			for (int32 z = 0; z < ChunkHightRange ; z++)
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
		BoundsMesh->SetRelativeScale3D(FVector(GetWorldLength() * BlockSize * 0.01f, GetWorldLength() * BlockSize * 0.01f, 15.f));
	}

	LastGenerateIndex = InIndex;
	ChunkSpawnBatch++;
}

void AWorldManager::SpawnChunk(FIndex InIndex, bool bAddToQueue)
{
	if (ChunkMap.Contains(InIndex)) return;

	FActorSpawnParameters spawnParams = FActorSpawnParameters();
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	auto chunk = GetWorld()->SpawnActor<AChunk>(InIndex.ToVector() * GetChunkLength(), FRotator::ZeroRotator, spawnParams);
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
}

void AWorldManager::BuildChunkMap(AChunk* InChunk)
{
	if (!InChunk || !ChunkMap.Contains(InChunk->GetIndex())) return;

	if (UDWHelper::GetWorldDataSave()->IsExistChunkData(InChunk->GetIndex()))
	{
		InChunk->LoadMap(UDWHelper::GetWorldDataSave()->LoadChunkData(InChunk->GetIndex()));
	}
	else
	{
		InChunk->BuildMap();
	}

	AddToGenerateQueue(InChunk);
}

void AWorldManager::GenerateChunk(AChunk* InChunk)
{
	if (!InChunk || !ChunkMap.Contains(InChunk->GetIndex())) return;
	
	InChunk->GenerateMesh();
}

void AWorldManager::DestroyChunk(AChunk* InChunk)
{
	if (!InChunk || !ChunkMap.Contains(InChunk->GetIndex())) return;

	InChunk->OnDestroy();

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

FChunkMaterial AWorldManager::GetChunkMaterial(ETransparency Transparency)
{
	return ChunkMaterials[FMath::Clamp((int32)Transparency, 0, ChunkMaterials.Num())];
}

EVoxelType AWorldManager::GetNoiseVoxelType(FIndex InIndex)
{
	FVector offsetIndex = FVector(InIndex.X + WorldSeed, InIndex.Y + WorldSeed, InIndex.Z);
	
	int plainHeight = GetNoiseTerrainHeight(offsetIndex, TerrainPlainScale);
	int mountainHeight = GetNoiseTerrainHeight(offsetIndex, TerrainMountainScale);
	
	int baseHeight = FMath::Clamp(FMath::Max(plainHeight, mountainHeight) + int(GetWorldHeight() * TerrainBaseHeight), 0, GetWorldHeight() - 1);

	int stoneHeight = FMath::Clamp(GetNoiseTerrainHeight(offsetIndex, TerrainStoneVoxelScale), 0, GetWorldHeight() - 1);
	int sandHeight = FMath::Clamp(GetNoiseTerrainHeight(offsetIndex, TerrainSandVoxelScale), 0, GetWorldHeight() - 1);

	int waterHeight = FMath::Clamp(int(GetWorldHeight() * TerrainWaterVoxelScale), 0, GetWorldHeight() - 1);
	int bedrockHeight = FMath::Clamp(int(GetWorldHeight() * TerrainBedrockVoxelScale), 0, GetWorldHeight() - 1);
	
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
	return UDWHelper::LoadVoxelData(*FString::Printf(TEXT("Voxel_%d"), (int)GetNoiseVoxelType(InIndex)));
}

int AWorldManager::GetNoiseTerrainHeight(FVector InOffset, FVector InScale)
{
	return (FMath::PerlinNoise2D(FVector2D(InOffset.X * InScale.X, InOffset.Y * InScale.Y)) + 1) * GetWorldHeight() * InScale.Z;
}

FTeamData* AWorldManager::GetTeamData(const FName& InTeamID)
{
	if (TeamMap.Contains(InTeamID))
	{
		return &TeamMap[InTeamID];
	}
	return &FTeamData::Empty;
}

bool AWorldManager::IsExistTeam(const FName& InTeamID)
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
			tmpData.Name = !InTeamName.IsNone() ? InTeamName : *FString::Printf(TEXT("%s �� Team"), &InCaptain->GetName());
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

AChunk* AWorldManager::FindChunk(FVector InLocation)
{
	return FindChunk(LocationToChunkIndex(InLocation));
}

AChunk* AWorldManager::FindChunk(FIndex InIndex)
{
	if (ChunkMap.Contains(InIndex))
		return ChunkMap[InIndex];
	return nullptr;
}

FIndex AWorldManager::LocationToChunkIndex(FVector InLocation, bool bIgnoreZ /*= false*/)
{
	FIndex chunkIndex = FIndex(FMath::FloorToInt(InLocation.X / GetChunkLength()),
		FMath::FloorToInt(InLocation.Y / GetChunkLength()),
		bIgnoreZ ? 0 : FMath::FloorToInt(InLocation.Z / GetChunkLength()));
	return chunkIndex;
}

FVector AWorldManager::ChunkIndexToLocation(FIndex InIndex)
{
	return InIndex.ToVector() * GetChunkLength();
}

float AWorldManager::GetChunkLength()
{
	return ChunkSize * BlockSize;
}

float AWorldManager::GetWorldLength()
{
	return ChunkSize * ChunkSpawnRange * 2;
}

int32 AWorldManager::GetWorldHeight()
{
	return ChunkSize * ChunkHightRange;
}

int32 AWorldManager::GetChunkDistance()
{
	return ChunkSpawnRange + ChunkSpawnDistance;
}

int32 AWorldManager::GetNumChunks(bool bNeedGenerated /*= false*/)
{
	int32 num = 0;
	for (auto iter = ChunkMap.CreateConstIterator(); iter; ++iter)
	{
		AChunk* chunk = iter->Value;
		if (!bNeedGenerated || chunk->IsGenerated())
			num++;
	}
	return num;
}

FVector AWorldManager::GetBlockSizedNormal(FVector InNormal, float InLength)
{
	return BlockSize * InNormal * InLength;
}

ETraceTypeQuery AWorldManager::GetGameTrace(EGameTraceType GameTraceType)
{
	return UEngineTypes::ConvertToTraceType((ECollisionChannel)GameTraceType);
}

bool AWorldManager::ChunkTraceSingle(AChunk* InChunk, float InRadius, float InHalfHeight, FHitResult& OutHitResult)
{
	FVector rayStart = InChunk->GetActorLocation() + FVector(FMath::FRandRange(1, ChunkSize - 1), FMath::FRandRange(1, ChunkSize), ChunkSize) * BlockSize;
	rayStart.X = ((int32)(rayStart.X / BlockSize) + 0.5f) * BlockSize;
	rayStart.Y = ((int32)(rayStart.Y / BlockSize) + 0.5f) * BlockSize;
	FVector rayEnd = rayStart + FVector::DownVector * GetChunkLength();
	return ChunkTraceSingle(rayStart, rayEnd, InRadius * 0.95f, InHalfHeight * 0.95f, OutHitResult);
}

bool AWorldManager::ChunkTraceSingle(FVector RayStart, FVector RayEnd, float InRadius, float InHalfHeight, FHitResult& OutHitResult)
{
	if (UKismetSystemLibrary::CapsuleTraceSingle(this, RayStart, RayEnd, InRadius, InHalfHeight, GetGameTrace(EGameTraceType::Chunk), false, TArray<AActor*>(), EDrawDebugTrace::None, OutHitResult, true))
		return OutHitResult.ImpactPoint.Z > 0;
	return false;
}

bool AWorldManager::VoxelTraceSingle(UVoxel* InVoxel, FVector InPoint, FHitResult& OutHitResult)
{
	FVector size = InVoxel->GetVoxelData().GetCeilRange(InVoxel) * BlockSize * 0.5f;
	return UKismetSystemLibrary::BoxTraceSingle(this, InPoint + size, InPoint + size, size * 0.95f, FRotator::ZeroRotator, GetGameTrace(EGameTraceType::Voxel), false, TArray<AActor*>(), EDrawDebugTrace::None, OutHitResult, true);
}

void AWorldManager::InitRandomStream(int32 InDeltaSeed)
{
	RandomStream.Initialize(WorldSeed + InDeltaSeed);
}
