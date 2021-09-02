// Fill out your copyright notice in the Description page of Project Settings.


#include "Voxel/Components/VoxelMeshComponent.h"
#include "World/WorldManager.h"
#include "Voxel/Voxel.h"
#include "World/Chunk.h"
#include "Materials/MaterialInstanceDynamic.h"

UVoxelMeshComponent::UVoxelMeshComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bUseAsyncCooking = true;
	BlockScale = 1;
	OffsetScale = 1;
	CenterOffset = FVector(0.5f);
	MeshType = EVoxelMeshType::Chunk;
	Transparency = ETransparency::Solid;
	Vertices = TArray<FVector>();
	Triangles = TArray<int32>();
	Normals = TArray<FVector>();
	UVs = TArray<FVector2D>();
	VertexColors = TArray<FColor>();
	Tangents = TArray<FProcMeshTangent>();
}

void UVoxelMeshComponent::Initialize(EVoxelMeshType InMeshType, ETransparency InTransparency /*= ETransparency::Solid*/)
{
	MeshType = InMeshType;
	Transparency = InTransparency;
	switch (InMeshType)
	{
		case EVoxelMeshType::Chunk:
		{
			BlockScale = 1;
			OffsetScale = 1;
			CenterOffset = FVector(0.5f);
			break;
		}
		case EVoxelMeshType::PickUp:
		{
			BlockScale = 0.3f;
			OffsetScale = 0;
			CenterOffset = FVector::ZeroVector;
			break;
		}
		case EVoxelMeshType::PreviewItem:
		{
			BlockScale = 0.3f;
			OffsetScale = 0;
			CenterOffset = FVector::ZeroVector;
			break;
		}
		case EVoxelMeshType::VitalityVoxel:
		{
			BlockScale = 1.5f;
			OffsetScale = 0;
			CenterOffset = FVector::ZeroVector;
			break;
		}
	}
}

void UVoxelMeshComponent::BuildVoxel(UVoxel* InVoxel)
{
	if (!UVoxel::IsValid(InVoxel, false)) return;

	if(InVoxel->GetVoxelData().bCustomMesh)
	{
		TArray<FVector> meshVertices, meshNormals;
		if (InVoxel->GetMeshDatas(meshVertices, meshNormals))
		{
			for (int i = 0; i < meshVertices.Num(); i++)
			{
				if (i > 0 && (i + 1) % 4 == 0)
				{
					FVector vertices[4];
					for (int j = 0; j < 4; j++)
					{
						vertices[j] = meshVertices[i - (3 - j)];
					}
					BuildFace(InVoxel, vertices, i / 4, meshNormals[i / 4]);
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < 6; i++)
		{
			if (InVoxel->CheckAdjacent((EDirection)i))
			{
				BuildFace(InVoxel, (EFacing)i);
			}
		}
	}
	if (MeshType == EVoxelMeshType::PickUp || MeshType == EVoxelMeshType::PreviewItem || MeshType == EVoxelMeshType::VitalityVoxel)
	{
		Transparency = InVoxel->GetVoxelData().Transparency;
	}
}

void UVoxelMeshComponent::CreateMesh(int InSectionIndex /*= 0*/, bool bHasCollider /*= true*/)
{
	if (Vertices.Num() > 0)
	{
		CreateMeshSection(InSectionIndex, Vertices, Triangles, Normals, UVs, VertexColors, Tangents, bHasCollider);
		UMaterialInterface* material = nullptr;
		switch (MeshType)
		{
			case EVoxelMeshType::Chunk:
			case EVoxelMeshType::PickUp:
			case EVoxelMeshType::VitalityVoxel:
			{
				material = AWorldManager::GetWorldInfo().GetChunkMaterial(Transparency).Material;
				break;
			}
			case EVoxelMeshType::PreviewItem:
			{
				material = UMaterialInstanceDynamic::Create(AWorldManager::GetWorldInfo().GetChunkMaterial(Transparency).Material, this);
				Cast<UMaterialInstanceDynamic>(material)->SetScalarParameterValue(TEXT("Emissive"), 1);
				break;
			}
		}
		SetMaterial(InSectionIndex, material);
		ClearData();
	}
	else
	{
		ClearMesh();
	}
}

void UVoxelMeshComponent::ClearMesh(int InSectionIndex /*= -1*/)
{
	if (GetNumSections() > 0)
	{
		if (InSectionIndex >= 0 && InSectionIndex < GetNumSections())
		{
			ClearMeshSection(InSectionIndex);
		}
		else
		{
			ClearAllMeshSections();
		}
	}
	ClearData();
}

void UVoxelMeshComponent::ClearData()
{
	Vertices.Empty();
	Triangles.Empty();
	Normals.Empty();
	UVs.Empty();
	VertexColors.Empty();
	Tangents.Empty();
}

void UVoxelMeshComponent::BuildFace(UVoxel* InVoxel, EFacing InFacing)
{
	FVector vertices[4];
	FVector range = InVoxel->GetVoxelData().GetFinalRange(InVoxel->GetRotation(), InVoxel->GetScale());

	switch (InFacing)
	{
		case EFacing::Up:
		{
			vertices[0] = FVector(-range.X, -range.Y, range.Z) * 0.5f;
			vertices[1] = FVector(range.X, -range.Y, range.Z) * 0.5f;
			vertices[2] = FVector(range.X, range.Y, range.Z) * 0.5f;
			vertices[3] = FVector(-range.X, range.Y, range.Z) * 0.5f;
			break;
		}
		case EFacing::Down:
		{
			vertices[0] = FVector(-range.X, range.Y, -range.Z) * 0.5f;
			vertices[1] = FVector(range.X, range.Y, -range.Z) * 0.5f;
			vertices[2] = FVector(range.X, -range.Y, -range.Z) * 0.5f;
			vertices[3] = FVector(-range.X, -range.Y, -range.Z) * 0.5f;
			break;
		}
		case EFacing::Forward:
		{
			vertices[0] = FVector(range.X, range.Y, -range.Z) * 0.5f;
			vertices[1] = FVector(range.X, range.Y, range.Z) * 0.5f;
			vertices[2] = FVector(range.X, -range.Y, range.Z) * 0.5f;
			vertices[3] = FVector(range.X, -range.Y, -range.Z) * 0.5f;
			break;
		}
		case EFacing::Back:
		{
			vertices[0] = FVector(-range.X, -range.Y, -range.Z) * 0.5f;
			vertices[1] = FVector(-range.X, -range.Y, range.Z) * 0.5f;
			vertices[2] = FVector(-range.X, range.Y, range.Z) * 0.5f;
			vertices[3] = FVector(-range.X, range.Y, -range.Z) * 0.5f;
			break;
		}
		case EFacing::Left:
		{
			vertices[0] = FVector(range.X, -range.Y, -range.Z) * 0.5f;
			vertices[1] = FVector(range.X, -range.Y, range.Z) * 0.5f;
			vertices[2] = FVector(-range.X, -range.Y, range.Z) * 0.5f;
			vertices[3] = FVector(-range.X, -range.Y, -range.Z) * 0.5f;
			break;
		}
		case EFacing::Right:
		{
			vertices[0] = FVector(-range.X, range.Y, -range.Z) * 0.5f;
			vertices[1] = FVector(-range.X, range.Y, range.Z) * 0.5f;
			vertices[2] = FVector(range.X, range.Y, range.Z) * 0.5f;
			vertices[3] = FVector(range.X, range.Y, -range.Z) * 0.5f;
			break;
		}
	}

	BuildFace(InVoxel, vertices, (int)InFacing, UDWHelper::DirectionToVector((EDirection)InFacing));
}

void UVoxelMeshComponent::BuildFace(UVoxel* InVoxel, FVector InVertices[4], int InFaceIndex, FVector InNormal)
{
	int32 verNum = Vertices.Num();
	FVoxelData voxelData = InVoxel->GetVoxelData();
	FVector2D uvCorner = voxelData.GetUVCorner(InFaceIndex, AWorldManager::GetWorldInfo().GetChunkMaterial(voxelData.Transparency).BlockUVSize);
	FVector2D uvSpan = voxelData.GetUVSpan(InFaceIndex, AWorldManager::GetWorldInfo().GetChunkMaterial(voxelData.Transparency).BlockUVSize);
	InNormal = InVoxel->GetRotation().RotateVector(InNormal);

	FVector center = voxelData.GetCeilRange(InVoxel) * (CenterOffset + InVoxel->GetRotation().RotateVector(voxelData.Offset) * OffsetScale);
	for (int32 i = 0; i < 4; i++)
	{
		Vertices.Add((InVoxel->GetIndex().ToVector() + center + InVoxel->GetRotation().RotateVector(InVertices[i] * InVoxel->GetScale())) * AWorldManager::GetWorldInfo().BlockSize * BlockScale);
	}

	UVs.Add(FVector2D(uvCorner.X, uvCorner.Y + uvSpan.Y));
	UVs.Add(uvCorner);
	UVs.Add(FVector2D(uvCorner.X + uvSpan.X, uvCorner.Y));
	UVs.Add(FVector2D(uvCorner.X + uvSpan.X, uvCorner.Y + uvSpan.Y));

	Triangles.Add(verNum + 1);
	Triangles.Add(verNum + 0);
	Triangles.Add(verNum + 2);
	Triangles.Add(verNum + 3);
	Triangles.Add(verNum + 2);
	Triangles.Add(verNum + 0);

	Normals.Add(InNormal);
	Normals.Add(InNormal);
	Normals.Add(InNormal);
	Normals.Add(InNormal);
}
