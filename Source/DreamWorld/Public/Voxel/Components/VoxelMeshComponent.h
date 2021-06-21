// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "ProceduralMeshComponent.h"
#include "VoxelMeshComponent.generated.h"

/**
 * �����������
 */
UCLASS(Blueprintable, ClassGroup = (Rendering, Common), hidecategories = Object, config = Engine, editinlinenew, meta = (BlueprintSpawnableComponent))
class DREAMWORLD_API UVoxelMeshComponent : public UProceduralMeshComponent
{
	GENERATED_BODY()
	
public:
	UVoxelMeshComponent(const FObjectInitializer& ObjectInitializer);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BlockScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float OffsetScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector CenterOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	EVoxelMeshType MeshType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	ETransparency Transparency;

private:
	TArray<FVector> Vertices;

	TArray<int32> Triangles;

	TArray<FVector> Normals;

	TArray<FVector2D> UVs;

	TArray<FColor> VertexColors;

	TArray<FProcMeshTangent> Tangents;

public:
	void Initialize(EVoxelMeshType InMeshType, ETransparency InTransparency = ETransparency::Solid);
	
	void BuildVoxel(UVoxel* InVoxel);

	void CreateMesh(int InSectionIndex = 0, bool bHasCollider = true);

	void ClearMesh(int InSectionIndex = -1);
	
private:
	void ClearData();

	void BuildFace(UVoxel* InVoxel, EFacing InFacing);

	void BuildFace(UVoxel* InVoxel, FVector InVertices[4], int InFaceIndex, FVector InNormal);
};